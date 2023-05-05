#ifndef NONARON_NONAHELPER_H
#define NONARON_NONAHELPER_H
#include <cmath>
#include <cstdint>
#include <string>
#include <Pinecone/Framework/Util/DOM/HTML/HTMLDocument.h>
#include <Pinecone/Framework/Util/DOM/HTML/Archetype/HTMLElement.h>
#include <Pinecone/Framework/Util/DOM/HTML/HTMLNodeList.h>

namespace Sauron {
    class _PINE_API_OUT NonaHelper {
    public:
        static std::string fmtDateTime( const char* pFmt ) ;

        static inline std::string getYMDHISDateTime() {
            return NonaHelper::fmtDateTime("%Y-%m-%d %H:%M:%S");
        }

        static inline std::string getYMDDateTime() {
            return NonaHelper::fmtDateTime("%Y-%m-%d");
        }

        static inline std::string getHISDateTime() {
            return NonaHelper::fmtDateTime("%H:%M:%S");
        }

        static inline Pinecone::HTMLElement* firstElement( const Pinecone::HTMLNodeList& nodes ) {
            if( !nodes.empty() ) {
                return nodes[ 0 ]->evinceHTMLElement();
            }
            return nullptr;
        }

    public:
        static void evalDocument ( const std::string& sz, Pinecone::HTMLDocument& document );

        static std::string innerText( const Pinecone::HTMLElement* that, const std::function<bool ( const Pinecone::HTMLElement* ) >& fnQualified );

        static std::string innerTextNoScriptStyle( const Pinecone::HTMLElement* that );
    };
}



#endif //NONARON_NONAHELPER_H
