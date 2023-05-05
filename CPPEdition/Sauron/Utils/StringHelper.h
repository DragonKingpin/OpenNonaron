#ifndef NONARON_STRINGHELPER_H
#define NONARON_STRINGHELPER_H
#include <cmath>
#include <cstdint>
#include <string>
#include <Pinecone/Framework/Unit/String/StdString.h>

namespace Sauron {
    class _PINE_API_OUT StringHelper {
    public:
        static std::string replaceAll ( const char* lpszParse, const char* lpszKey, const char* lpszReplace, size_t nReplace ) ;

        inline static std::string replaceAll ( const char* lpszParse, const char* lpszKey, const char* lpszReplace ) {
            return StringHelper::replaceAll( lpszParse, lpszKey, lpszReplace, Pinecone::StringStereotype::traits_type::length( lpszReplace ) );
        }

        inline static std::string replaceAll ( const char* lpszParse, const char* lpszKey, const std::string& szReplace ) {
            return StringHelper::replaceAll( lpszParse, lpszKey, szReplace.c_str(), szReplace.length() );
        }

        inline static std::string replaceAll ( const std::string& szParse, const char* lpszKey, const std::string& szReplace ) {
            return StringHelper::replaceAll( szParse.c_str(), lpszKey, szReplace.c_str(), szReplace.length() );
        }

        static std::string trimFront            ( const std::string& szParse ) ;

        static std::string getDirectoryByPath   ( const char* lpszParse );

        static bool endsWith ( const std::string& szStr , const std::string& szSuffix );

        static std::string readAllFromPoint( std::istream& in, std::size_t nStreamSize = 1024 );

        static std::string utf8MB3Normalize( const char* lpsz, size_t nLength ) ;
    };

    class _PINE_API_OUT ChainStringFormatter {
    private:
        const char* mpszToParse  ;
        const char* mpszParseAt  ;
        std::string mszFormatted ;

    public:
        ChainStringFormatter(): mpszToParse( nullptr ), mpszParseAt( nullptr ) {}

        explicit ChainStringFormatter( const char* lpszParse ):
                mpszToParse( lpszParse ), mpszParseAt( lpszParse ) {
        }

        explicit ChainStringFormatter( const std::string& szParse ): ChainStringFormatter( szParse.c_str() ) {

        }

    public:
        ChainStringFormatter* apply ( const char* lpszParse ) {
            this->mpszToParse = lpszParse;
            this->mpszParseAt = lpszParse;
            return this;
        }

        ChainStringFormatter* apply ( const std::string& szParse ) {
            return this->apply( szParse.c_str() );
        }

        void clear() _PINE_NOEXCEPT {
            this->mpszParseAt = this->mpszToParse;
            this->mszFormatted.clear();
        }

        ChainStringFormatter* format( const char* lpszKey, const char* lpszReplace, size_t nReplace ) ;

        ChainStringFormatter* format( const char* lpszKey, const char* lpszReplace ) {
            return this->format( lpszKey, lpszReplace, Pinecone::StringStereotype::traits_type::length( lpszReplace ) );
        }

        ChainStringFormatter* format( const char* lpszKey, const std::string& szReplace ) {
            return this->format( lpszKey, szReplace.c_str(), szReplace.length() );
        }

        std::string& eval() {
            this->mpszParseAt = this->mpszToParse;
            return this->mszFormatted;
        }

    };

    class _PINE_API_OUT HttpURLParser {
    public:
        // Prototype from MicrosoftWindows::URL_COMPONENTS, implemented in pure C++ for across platform.
        struct URLComponents {
            const char* lpszScheme       = "" ;
            size_t      nSchemeLength    = 0  ;
            const char* lpszHostName     = "" ;
            size_t      nHostNameLength  = 0  ;
            int16_t     nPort            = -1 ;
            const char* lpszUserName     = "" ;
            size_t      nUserNameLength  = 0  ;
            const char* lpszPassword     = "" ;
            size_t      nPasswordLength  = 0  ;
            const char* lpszUrlPath      = "" ;
            size_t      nUrlPathLength   = 0  ;
            const char* lpszExtraInfo    = "" ;
            size_t      nExtraInfoLength = 0  ;
        };

    public:
        inline static void crackUrl ( const std::string& szURL, URLComponents* that ) {
            HttpURLParser::crackUrl( szURL.c_str(), szURL.length(), that );
        }

        static void crackUrl ( const char* lpszURL, size_t nURLLength, URLComponents* that );

    protected:
        std::string        mszParentURL        ;
        std::string        mszSubURL           ;
        bool               mbIsDifferentDomain  = false;

        URLComponents      mParentFrags;
        URLComponents      mSubFrags;

    protected:
        void profiling();

    public:
        HttpURLParser( const std::string& szParentURL, const std::string& szThat ) {
            this->mszParentURL = szParentURL;
            this->mszSubURL    = szThat     ;
            this->profiling();
        }

    public:
        std::string evalRelatedMajorHref() const;

        bool isDifferentDomain() const _PINE_NOEXCEPT {
            return this->mbIsDifferentDomain;
        }
    };
}


#endif //NONARON_STRINGHELPER_H
