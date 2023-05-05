#include <fstream>
#include <Pinecone/Framework/Unit/String/Stereotype/BasicStringStereotype.h>
#include <Pinecone/Framework/System/Library/stdlib.fun.h>
#include <Sauron/System/defined.inc.h>
#include "StringHelper.h"

namespace Sauron {
    std::string StringHelper::replaceAll( const char *lpszParse, const char *lpszKey, const char *lpszReplace, size_t nReplace ) {
        std::string sz;
        while ( lpszParse && *lpszParse ) {
            const char* pNextAt = nullptr;
            if( *lpszParse == *lpszKey ) {
                const char* lpKeyCmp = lpszKey;
                const char* lpParseCmp = lpszParse;
                while ( *lpParseCmp ) {
                    if( !*lpKeyCmp ) {
                        pNextAt = lpParseCmp;
                    }
                    if( *lpParseCmp != *lpKeyCmp ) {
                        break;
                    }

                    ++lpParseCmp;
                    ++lpKeyCmp;

                    if( !*lpKeyCmp && !*lpParseCmp ) {
                        pNextAt = lpParseCmp;
                    }
                }

                if( pNextAt ) {
                    sz.append( lpszReplace, nReplace );
                    lpszParse = pNextAt;
                    continue;
                }
            }

            sz.push_back( *lpszParse );
            ++lpszParse;
        }

        return sz;
    }

    std::string StringHelper::trimFront( const std::string &szParse ) {
        std::string sz;
        const char* lpszParseLeg = szParse.c_str();
        const char* lpszParseAt  = lpszParseLeg;

        while ( lpszParseAt && *lpszParseAt == ' ' ) {
            ++lpszParseAt;
        }

        sz.assign( lpszParseAt,szParse.length() - (lpszParseAt - lpszParseLeg) );
        return sz;
    }

    std::string StringHelper::getDirectoryByPath( const char *lpszParse ) {
        const char* lpszDirLeg = lpszParse;
        const char* lpszDirAt  = lpszParse;
        while ( lpszParse && *lpszParse ) {
            if( *lpszParse == '/' || *lpszParse == '\\' ){
                lpszDirAt = lpszParse;
            }
            ++lpszParse;
        }

        std::string sz = std::string( lpszDirLeg, lpszDirAt - lpszDirLeg );
        return sz;
    }

    bool StringHelper::endsWith( const std::string &szStr, const std::string &szSuffix ) {
        size_t nLen = szStr.length();
        const char* lpszParerAt = szStr.c_str() + nLen - 1;

        while ( nLen && *lpszParerAt ) {
            if( !std::strchr( " \n\r\t", *lpszParerAt ) ){
                ++nLen;
                ++lpszParerAt;
                break;
            }
            --nLen;
            --lpszParerAt;
        }

        size_t nThatLen = szSuffix.length();
        if( nThatLen > nLen ){
            return false;
        }

        lpszParerAt -= nThatLen;
        const char* lpszThatAt = szSuffix.c_str();
        for ( size_t i = nThatLen - 1; i > 0; --i ) {
            if( lpszParerAt[i] != lpszThatAt[i] ) {
                return false;
            }
        }
        return true;
    }

    std::string StringHelper::readAllFromPoint( std::istream &in, std::size_t nStreamSize ) {
        std::string ret;
        ret.reserve( nStreamSize * 2 );

        std::ifstream::char_type* buffer = new std::ifstream::char_type [ nStreamSize ];
        std::streamsize bufSize = (std::streamsize) ( sizeof(std::ifstream::char_type) * nStreamSize );

        while ( in.read(buffer, bufSize) ){
            //buffer[bufSize-1] = 0;
            ret.append(buffer, bufSize);
        }

        ret.append( buffer, in.gcount() );
        delete[] buffer;
        return ret;
    }

    std::string StringHelper::utf8MB3Normalize( const char *lpsz, size_t nLength ) {
        std::string res;
        res.reserve( nLength );
        byte c, c2, c3, c4;

        for ( size_t i = 0 ; i < nLength; ++i ) {
            c = (byte)lpsz[ i ];
            if ( c < 32 ){ // Control char
                if ( c == 9 || c == 10 || c == 13 ){ // Allow only \t \n \r
                    res.push_back( (char) c );
                }
                continue;
            }
            else if ( c < 127 ) { // Normal ASCII
                res.push_back( (char) c );
                continue;
            }
            else if ( c < 160 ) {  // Control char (nothing should be defined here either ASCI, ISO_8859-1 or UTF8, so skipping)
                if ( c2 == 128 ) {  // Fix microsoft mess, add euro
                    res.push_back( (char) 226 );
                    res.push_back( (char) 130 );
                    res.push_back( (char) 172 );
                }
                if ( c2 == 133 ){ // Fix IBM mess, add NEL = \n\r
                    res.push_back( (char) 10 );
                    res.push_back( (char) 13 );
                }
                continue;
            }
            else if ( c < 192 ){  // Invalid for UTF8, converting ASCII
                res.push_back( (char) (byte)194 );
                res.push_back( (char) c );
                continue;
            }
            else if ( c < 194 ){  // Invalid for UTF8, converting ASCII
                res.push_back( (char) (byte)195  );
                res.push_back( (char) ( c - 64 ) );
                continue;
            }
            else if ( c < 224 && i + 1 < nLength ) { // Possibly 2byte UTF8
                c2 = (byte) lpsz[ i + 1 ];
                if ( c2 > 127 && c2 < 192 ) { // Valid 2byte UTF8
                    if ( !( c == 194 && c2 < 160 ) ) { // Control char, skipping
                        res.push_back( (char) c  );
                        res.push_back( (char) c2 );
                    }
                    ++i;
                    continue;
                }
            }
            else if( c < 240 && i + 2 < nLength ){  // Possibly 3byte UTF8
                c2 = (byte)lpsz[ i + 1 ];
                c3 = (byte)lpsz[ i + 2 ];
                if( c2 > 127 && c2 < 192 && c3 > 127 && c3 < 192 ){//valid 3byte UTF8
                    res.push_back( (char) c  );
                    res.push_back( (char) c2 );
                    res.push_back( (char) c3 );
                    i += 2;
                    continue;
                }
            }
            else if( c < 245 && i + 3 < nLength ){  // Possibly 4byte UTF8
                continue; // Skip utf8mb4
//                c2 = (byte)lpsz[ i + 1 ];
//                c3 = (byte)lpsz[ i + 2 ];
//                c4 = (byte)lpsz[ i + 3 ];
//                if( c2 > 127 && c2 < 192 && c3 > 127 && c3 < 192 && c4 > 127 && c4 < 192 ) { // Valid 4byte UTF8
//                    res.push_back( (char) c  );
//                    res.push_back( (char) c2 );
//                    res.push_back( (char) c3 );
//                    res.push_back( (char) c4 );
//                    i += 3;
//                    continue;
//                }
            }

            // Invalid UTF8, converting ASCII (c>245 || string too short for multi-byte))
            res.push_back( (char) (byte) 195 );
            res.push_back( (char) ( c - 64 ) );
        }
        return res;
    }

    ChainStringFormatter *ChainStringFormatter::format( const char *lpszKey, const char* lpszReplace, size_t nReplace ) {
        this->mszFormatted = StringHelper::replaceAll( this->mpszParseAt, lpszKey, lpszReplace, nReplace );
        this->mpszParseAt  = this->mszFormatted.c_str();
        return this;
    }

    void HttpURLParser::crackUrl( const char *lpszURL, size_t nURLLength, URLComponents *that ) {
        const char* lpszHostAt = nullptr;

        for ( size_t i = 0; i < nURLLength;  ) {
            const char* lpAt = lpszURL + i;

            if( i + 2 < nURLLength && lpszURL[i] == ':' && lpszURL[i + 1] == '/' && lpszURL[i + 2] == '/' ) {
                that->lpszScheme    = lpszURL;
                that->nSchemeLength = i;
                i += 1; // Skip ':'
                lpszHostAt = lpszURL + i + 2;
                continue;
            }
            else if( that->nUrlPathLength == 0 && lpszURL[i] == '/' && lpszURL[i + 1] == '/' ) {
                for ( size_t j = i; j < nURLLength; ++j ) {
                    if( lpszURL[ j ] == '@' ){
                        for ( size_t k = j; k ; --k ) {
                            if( lpszURL[ k ] == ':' ){
                                lpAt += 2; // Skip '//'
                                that->nPasswordLength = j - k - 1; // -1 for ':'
                                that->lpszPassword    = lpszURL + k + 1;

                                that->lpszUserName    = lpAt;
                                that->nUserNameLength = that->lpszPassword - that->lpszUserName - 1; // -1 for ':'

                                lpszHostAt = lpszURL + j + 1;
                                break;
                            }
                        }

                        i = j;
                        break;
                    }
                }

                if( !that->nUserNameLength ) {
                    lpszHostAt = lpAt + 2;
                    i += 2;
                    size_t j;
                    for ( j = i; j < nURLLength; ++j ) {
                        if( lpszURL[ j ] == '/' ){
                            i = j;
                            break;
                        }
                    }
                    if( j == nURLLength ){
                        that->lpszHostName    = lpszHostAt;
                        that->nHostNameLength = j - i;
                        break;
                    }
                    continue;
                }
            }
            else if( that->nUrlPathLength == 0 && std::strchr( "./?&", lpszURL[i] ) ) {
                size_t nPortLen = 0;
                if( std::isdigit( lpszURL[ i - 1 ] ) ) {
                    for ( size_t j = i; j ; --j ) {
                        if( lpszURL[ j ] == ':' ){
                            nPortLen = i - j - 1;
                            that->nPort = _PINE_ATOI( lpAt - nPortLen );
                            break;
                        }
                    }
                }

                that->lpszUrlPath     = lpAt;
                that->nUrlPathLength  = nURLLength - i;
                if( lpszHostAt ) {
                    that->lpszHostName    = lpszHostAt;
                    that->nHostNameLength = that->lpszUrlPath - that->lpszHostName;
                }
                if( nPortLen ) {
                    that->nHostNameLength = that->nHostNameLength - nPortLen - 1;
                }
                break;
            }
            ++i;
        }
    }


    void HttpURLParser::profiling() {
        HttpURLParser::crackUrl( this->mszParentURL.c_str(), this->mszParentURL.length(), &this->mParentFrags );
        HttpURLParser::crackUrl( this->mszSubURL.c_str(), this->mszSubURL.length(), &this->mSubFrags );

        if( this->mParentFrags.nPort != this->mSubFrags.nPort ) {
            this->mbIsDifferentDomain = true;
            return;
        }

        if( this->mSubFrags.nSchemeLength ) {
            this->mbIsDifferentDomain = true;
            return;
        }

        if( this->mSubFrags.nHostNameLength ){
            std::string szParentDomainPart ( this->mParentFrags.lpszUserName, this->mParentFrags.nUserNameLength );
            szParentDomainPart.append( this->mParentFrags.lpszPassword, this->mParentFrags.nPasswordLength );
            szParentDomainPart.append( this->mParentFrags.lpszHostName, this->mParentFrags.nHostNameLength );

            std::string szThatDomainPart ( this->mSubFrags.lpszUserName, this->mSubFrags.nUserNameLength );
            szThatDomainPart.append( this->mSubFrags.lpszPassword, this->mSubFrags.nPasswordLength );
            szThatDomainPart.append( this->mSubFrags.lpszHostName, this->mSubFrags.nHostNameLength );

            if( szParentDomainPart != szThatDomainPart ) {
                this->mbIsDifferentDomain = true;
                return;
            }
        }
    }

    std::string HttpURLParser::evalRelatedMajorHref( ) const {
        if( !this->mbIsDifferentDomain ){
            if( this->mSubFrags.lpszUrlPath[0] != '/' ) {
                std::string sz ( this->mParentFrags.lpszUrlPath, this->mParentFrags.nUrlPathLength );
                if( sz.back() == '/' && this->mSubFrags.lpszUrlPath[0] == '/' ){
                    sz.pop_back();
                }
                if( this->mSubFrags.lpszUrlPath[0] == '.' ) {
                    sz.append( this->mSubFrags.lpszUrlPath + 1, this->mSubFrags.nUrlPathLength - 1 );
                }
                else {
                    sz.append( this->mSubFrags.lpszUrlPath, this->mSubFrags.nUrlPathLength );
                }
                return sz;
            }
        }

        std::string sz ( this->mSubFrags.lpszUrlPath, this->mSubFrags.nUrlPathLength );
        return sz;
    }
}