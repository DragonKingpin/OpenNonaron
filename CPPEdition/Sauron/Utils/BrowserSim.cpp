#include <Pinecone/Framework/Element/Atom/Atom.h>
#include <Sauron/System/System.h>
#include "BrowserSim.h"
#include <Pinecone/Extension/Util/Net/lib_http.h>
#include <windows.h>
#include <shlwapi.h>
#include <wininet.h>
#include <Pinecone/Framework/Util/Random/Random.h>
#include <Sauron/Heistron/Reaver.h>

namespace Sauron {
    std::string BrowserSim::getHTTPFile( const std::string &szHref ) const {
        std::ostringstream ss;
        this->getAccept( szHref, ss );
        return ss.str();
    }


    InnerBrowserSim::InnerBrowserSim( System *lpSys ) {
        this->apply( lpSys->queryDefaultBrowserSimConfig() );
    }

    InnerBrowserSim::InnerBrowserSim( HTTPReaver *lpReaver ) {
        this->apply( lpReaver->queryBrowserConfig() );
    }

    InnerBrowserSim *InnerBrowserSim::apply( PineAtom* lpConf ) {
        this->mpProtoConfig         = lpConf;
        this->mpHeadConfigGroup     = &(*this->mpProtoConfig)     [ "HeadConfigGroup"   ];
        Pinecone::Atom& jConf       = *this->mpProtoConfig;

        this->mbGroupConfusion      = (bool)    jConf[ "GroupConfusion"     ].getRawInt64Zone() ;
        this->mbEnableRandomDelay   = (bool)    jConf[ "EnableRandomDelay"  ].getRawInt64Zone() ;
        this->mnRandomDelayMin      = (int32_t) jConf[ "RandomDelayMin"     ].getRawInt64Zone() ;
        this->mnRandomDelayMax      = (int32_t) jConf[ "RandomDelayMax"     ].getRawInt64Zone() ;

        return this;
    }

    void InnerBrowserSim::beforeNextHttpQuery() const {

    }

    void InnerBrowserSim::afterLastHttpQuery() const {
        if( this->mbEnableRandomDelay ) {
            std::this_thread::sleep_for( std::chrono::milliseconds(
                    Pinecone::Random::nextInt32(
                            this->mnRandomDelayMin, this->mnRandomDelayMax
                    )
            ) );
        }
    }

    void InnerBrowserSim::getAccept( const std::string &szHref, fnAfterBufferReceived_t &&fnAfterBufferReceived ) const {
        this->beforeNextHttpQuery();

        PineAtom& jOpts  = *this->mpHeadConfigGroup;
        PineAtom* lpOpt  ;
        if( this->mbGroupConfusion ) {
            lpOpt        = &jOpts[ Pinecone::Random::nextInt32( 0, (int32_t)jOpts.size() - 1 ) ];
        }
        else {
            lpOpt        = &jOpts[ 0 ];
        }
        PineAtom& jOpt   = *lpOpt;

        size_t      nMaxHTTPPackageSize  = (size_t) jOpt[ "MaxHTTPPackageSize"     ].getRawInt64Zone();
        std::string szAgentName          = jOpt[ "AgentName"              ].toString();
        std::string szUserAgentHead      = jOpt[ "UserAgentHead"          ].toString();

        char* contentBuf = new char[ nMaxHTTPPackageSize ]{ 0 };
        ULONG sizeofBuf = 1;
        HINTERNET hSession = InternetOpenA( szAgentName.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0 );

        const char* lpszHead = szUserAgentHead.c_str();
        if ( hSession ) {
            HINTERNET handle2 = InternetOpenUrlA(hSession, szHref.c_str(), lpszHead, 0, INTERNET_FLAG_DONT_CACHE, 0);
            if (handle2) {
                bool flag;
                do {
                    flag = InternetReadFile(handle2, contentBuf, nMaxHTTPPackageSize - 1, &sizeofBuf);
                    if( fnAfterBufferReceived( (char*)contentBuf, sizeofBuf ) < 0 ){
                        break;
                    }
                }
                while ( flag && sizeofBuf > 0 );
                InternetCloseHandle(handle2);
            }
            InternetCloseHandle(hSession);
        }

        delete[] contentBuf;

        this->afterLastHttpQuery();
    }

    void InnerBrowserSim::getAccept( const std::string &szHref, std::ostream &ofs ) const {
        std::ostringstream ss;
        this->getAccept( szHref, [&]( char* lpszContentBuf, size_t nSizeofBuf ){
            ss.write( lpszContentBuf, (std::streamsize) nSizeofBuf );
            return 1;
        });
    }

    std::string InnerBrowserSim::getHTTPFile( const std::string &szHref ) const {
        std::string szHtml;
        szHtml.reserve( PINE_MB( 1 ) );
        this->getAccept( szHref, [&]( char* lpszContentBuf, size_t nSizeofBuf ){
            szHtml.append( lpszContentBuf, nSizeofBuf );
            return 1;
        });
        return szHtml;
    }
}

