#include <Pinecone/Framework/Element/Atom/Atom.h>
#include <Pinecone/Framework/Unit/Dictron/Dictron.h>
#include <Pinecone/Framework/Unit/Dictron/Maptron.h>
#include <Sauron/System/System.h>
#include <Pinecone/Framework/Util/File/FileUtils.h>
#include <Sauron/System/defined.inc.h>
#include <Sauron/System/Clique/CliqueAffairs.h>
#include <Sauron/Utils/Semantic/DragronExpressionParser.h>
#include "PathRouter.h"

namespace Sauron {
    PathRouter::PathRouter( System *lpSystem ) : mpSystem( lpSystem ) {
        PineAtom& joSystemConf = *this->mpSystem->getSystemConfig();
        PineAtom& joPathRouter = joSystemConf[ "PathRouter" ];
        if( joPathRouter.isString() ){
            std::string szConfPath = this->mpSystem->getWorkingPath() + joPathRouter.toString();
            joPathRouter.jsonDecode( Pinecone::FileUtils::readAll( szConfPath ) );
        }

        this->mpjoPathRouter        = &joPathRouter;
        this->mpjoReinterpretTab    = &joPathRouter[ "Reinterpret" ];
        this->mpReTabDictionary     = new PineAtom ;
        this->reinterpretReTab();
    }

    PathRouter::~PathRouter() {
        delete this->mpReTabDictionary;
    }

    void PathRouter::reinterpretReTab() {
        PineAtom& joSystem = *this->mpSystem->getSystemConfig();
        for ( auto& kv : *joSystem.affirmDictron()->affirmMaptron() ) {
            (*this->mpReTabDictionary)[ kv.Key ].referenceAssign( kv.Value );
        }

        for ( auto& kv : *this->mpSystem->getCliqueAffairs()->getNickNameMap()->affirmDictron()->affirmMaptron() ) {
            (*this->mpReTabDictionary)[ kv.Key ].referenceAssign( kv.Value[ "LocalDomain" ] );
        }

        for ( auto& kv : *this->mpjoReinterpretTab->affirmDictron()->affirmMaptron() ) {
            if( this->mpReTabDictionary->hasOwnProperty( kv.Key ) ) {
                throw Pinecone::IllegalArgumentException( "IllegalArgumentException: Illegal system config, duplicated config key.", __ERROR_AT__ );
            }
            if( kv.Value.isString() ) {
                std::string szRaw = kv.Value.toString();
                DragronExpressionParser parser( this->mpReTabDictionary, szRaw.c_str() );
                (*this->mpReTabDictionary)[ kv.Key ] = parser.eval();
            }
            else {
                throw Pinecone::IllegalArgumentException( "IllegalArgumentException: Illegal system config, reinterpret key can not be object.", __ERROR_AT__ );
            }
        }
    }
}