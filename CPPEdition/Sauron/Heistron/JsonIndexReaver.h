#ifndef NONARON_JSONINDEXREAVER_H
#define NONARON_JSONINDEXREAVER_H
#include <Pinecone/Framework/defined.inc.h>
#include <Ulfhedinn/config.inc.h>
#include <Pinecone/Framework/Unit/String/StdString.h>
#include <Pinecone/Framework/Util/Tracer/Tracer.h>
#include "Sauron/Heistron/Reaver.h"

namespace Pinecone {
    class Atom;

    class Listron;

    class HTMLDocument;
}

namespace Sauron {
    class Heist;

    _PINE_ABSTRACT_CLASS _PINE_API_OUT JsonIndexReaver : _PINE_IMPLEMENTS public MegaDOMIndexReaver {
    protected:
        mutable size_t        mnLastStalkerId = 0    ;
        Pinecone::Listron*    mpIndexList            ;

    public:
        explicit JsonIndexReaver( Heist* lpDispatcher ) ;

        JsonIndexReaver( Heist* lpDispatcher, size_t nLootFrom  , size_t nLootTo ) ;

    public:
        void validateSpoil( const std::string& sz ) const _PINE_OVERRIDE ;

        std::string queryHrefById ( size_t id ) const _PINE_OVERRIDE ;

    protected:
        void revitalizeStalker() const _PINE_OVERRIDE ;

    public:
        void toStalk() const _PINE_OVERRIDE ;

        void toEmbezzle() const _PINE_OVERRIDE ;

        void isTimeToFeast() const _PINE_OVERRIDE ;

    public:
        std::string prototypeName() const _PINE_OVERRIDE {
            return this->className();
        }

        std::string className() const _PINE_OVERRIDE = 0;

        std::string crewName() const _PINE_OVERRIDE  = 0;

    };
}



#endif //NONARON_JSONINDEXREAVER_H
