#ifndef NONARON_SITEMAPSQLREAVER_H
#define NONARON_SITEMAPSQLREAVER_H
#include <Pinecone/Framework/defined.inc.h>
#include "Sauron/Heistron/Reaver.h"

namespace Pinecone {
    class Atom;

    class HTMLDocument;
}

namespace Sauron {
    class Heist;

    _PINE_ABSTRACT_CLASS _PINE_API_OUT SitemapSQLReaver : _PINE_EXTENDS public MegaDOMIndexReaver {
    protected:
        std::string           mszSitemapApiHref      ;
        std::string           mszMapIndexPath        ;
        mutable size_t        mnLastStalkerId = 0    ;

    protected:
        bool                  mbUsingBuffIndexQuery  ;
        size_t                mnBuffIndexQuerySize   ;

    public:
        explicit SitemapSQLReaver( Heist* lpDispatcher ) ;

        SitemapSQLReaver( Heist* lpDispatcher, size_t nLootFrom  , size_t nLootTo ) ;

    public:
        std::string queryHrefById ( size_t id ) const _PINE_OVERRIDE;

    public:
        void revitalizeStalker() const _PINE_OVERRIDE;

    public:
        void isTimeToFeast() const _PINE_OVERRIDE ;

    };
}

#endif //NONARON_SITEMAPSQLREAVER_H
