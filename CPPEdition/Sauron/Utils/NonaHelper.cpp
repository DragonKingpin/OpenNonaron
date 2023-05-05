#include <ctime>
#include <Pinecone/Framework/Util/DOM/HTML/HTMLParser.h>
#include <Pinecone/Framework/Util/DOM/HTML/Util/HTMLToken.h>
#include <Pinecone/Framework/Util/DOM/HTML/Archetype/HTMLText.h>
#include "NonaHelper.h"

namespace Sauron {
    std::string NonaHelper::fmtDateTime( const char *pFmt ) {
        char tmp[128];
        std::time_t nTime = std::time(nullptr);
        std::strftime( tmp, sizeof(tmp), pFmt, std::localtime( &nTime ) );
        return tmp;
    }

    void NonaHelper::evalDocument( const std::string &sz, Pinecone::HTMLDocument& document ) {
        Pinecone::HTMLParser parser( sz );
        parser.parse( &document );
    }

    std::string NonaHelper::innerText( const Pinecone::HTMLElement* that, const std::function<bool(const Pinecone::HTMLElement *) > &fnQualified ) {
        std::stringstream ss;

        for ( const Pinecone::HTMLTreeNode* node = that->firstChild(); node; node = (const Pinecone::HTMLTreeNode*)node->nextSibling() ) {
            const Pinecone::HTMLText*     pText    = node->evinceHTMLText();
            const Pinecone::HTMLElement*  pElement = node->evinceHTMLElement();
            if( pElement && !fnQualified( pElement ) ) {
                continue;
            }

            if( pText ) {
                if( !Pinecone::HTMLToken::prospectCurrentTokenIsHTMLNewLine ( pText->data().c_str(), pText->data().length() ) ){
                    pText->serialize ( ss, 0 );
                }
            }
            else if( pElement ) {
                ss << ' ';
                ss << NonaHelper::innerText( pElement, fnQualified );
            }
        }

        return ss.str();
    }

    std::string NonaHelper::innerTextNoScriptStyle( const Pinecone::HTMLElement *that ) {
        return NonaHelper::innerText( that, []( const Pinecone::HTMLElement* that ) {
            return that->elementType() != Pinecone::HTMLElement::ElementType::T_ELE_SCRIPT &&
            that->elementType() != Pinecone::HTMLElement::ElementType::T_ELE_STYLE;
        } );
    }
}
