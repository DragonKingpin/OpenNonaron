#include <Pinecone/Framework/Element/Atom/Atom.h>
#include <Pinecone/Framework/Util/JSON/JSON.h>
#include <Pinecone/Framework/Util/File/FileUtils.h>
#include <Pinecone/Framework/Util/Tracer/Tracer.h>
#include <Pinecone/Framework/Util/DOM/HTML/HTMLDocument.h>
#include <shlwapi.h>
#include "Reavers.h"
#include "Sauron/Heistron/Heist.h"
#include "SitemapSQLReaver.h"

namespace Sauron {
    SitemapSQLReaver::SitemapSQLReaver( Heist *lpDispatcher ) : MegaDOMIndexReaver( lpDispatcher ) {
        this->mszSitemapApiHref        = this->mpParentHeistum->queryConfig( "SitemapApiHref" )->toString();
        this->mszMapIndexPath          = this->mpParentHeistum->queryConfig( "MapIndexPath"   )->toString();
        this->mbUsingBuffIndexQuery    = (bool)   this->mpParentHeistum->queryConfig( "UsingBuffIdxQuery"   )->getRawInt64Zone();
        this->mnBuffIndexQuerySize     = (size_t) this->mpParentHeistum->queryConfig( "BuffIdxQuerySize"    )->getRawInt64Zone();
    }

    SitemapSQLReaver::SitemapSQLReaver(
            Heist *lpDispatcher, size_t nLootFrom, size_t nLootTo
    ) : SitemapSQLReaver( lpDispatcher ) {
        this->mnLootFrom        = nLootFrom;
        this->mnLootTo          = nLootTo;
        //this->mpIndexList       = this->mpParentHeistum->mjIndex.affirmDictron()->affirmListron();
    }



    std::string SitemapSQLReaver::queryHrefById( size_t id ) const {
        return "";
    }



    void SitemapSQLReaver::revitalizeStalker() const {
        if( this->mpParentHeistum->isFromDeathPoint() ) {
            std::string szDyingMsgFileName = this->mpParentHeistum->getDyingMsgFileName();
        }
    }

    void SitemapSQLReaver::isTimeToFeast() const {
        this->MegaDOMIndexReaver::isTimeToFeast();
    }

}