#include <Pinecone/Framework/Element/Atom/Atom.h>
#include <Pinecone/Framework/Unit/Dictron/Dictron.h>
#include <Pinecone/Framework/Util/DOM/HTML/HTMLDocument.h>
#include <Pinecone/Framework/Util/File/FileUtils.h>
#include <Pinecone/Framework/Util/DateTime/DateTime.h>
#include <shlwapi.h>
#include <Sauron/Utils/IndexSniffer/ParadigmSniffer.h>
#include "Sauron/Heistron/Heist.h"
#include "Reavers.h"
#include "JsonIndexReaver.h"

namespace Sauron {
    JsonIndexReaver::JsonIndexReaver( Heist *lpDispatcher ) : MegaDOMIndexReaver( lpDispatcher ) {
        this->mpIndexList       = nullptr;

        if ( !this->mpIndexSniffer ) {
            this->mpIndexSniffer = ParadigmSniffer::spawn( this );
        }
    }

    JsonIndexReaver::JsonIndexReaver(
            Heist *lpDispatcher, size_t nLootFrom, size_t nLootTo
    ) : JsonIndexReaver( lpDispatcher ) {
        this->mnLootFrom        = nLootFrom;
        this->mnLootTo          = nLootTo;
        this->mpIndexList       = this->mpParentHeistum->mjIndex.affirmDictron()->affirmListron();
    }

    std::string JsonIndexReaver::queryHrefById( size_t id ) const {
        return *( (*this->mpIndexList)[ id ] ).getRawStringZone();
    }

    void JsonIndexReaver::validateSpoil( const std::string &sz ) const {
        if( sz.length() < this->mFailureConf.nFailedFileSize ) {
            throw Pinecone::IllegalStateException( "CompromisedFilesSize." );
        }
        if( !Pinecone::StdString::endsWith( sz, "</html>" ) ) {
            throw Pinecone::IllegalStateException( "NetworkError::IncompleteFiles." );
        }
    }

    void JsonIndexReaver::revitalizeStalker() const {
        if( this->mpParentHeistum->isFromDeathPoint() ) {
            Pinecone::Atom atom = this->mpParentHeistum->queryDefaultTomb();
            if( atom.isDictron() ) {
                std::string szIndexApiHref = atom[ "href" ].toString().substr( this->mszHeistHref.length() );
                this->mnLastStalkerId      = atom[ "id"   ].getRawInt64Zone();
                this->mpIndexSniffer->revitalize( this->mnLastStalkerId, szIndexApiHref );

                this->mpParentHeistum->trace( { "[Successfully revitalized]", this->mnLastStalkerId, szIndexApiHref } );
            }
        }
    }

    void JsonIndexReaver::toStalk() const {
        this->revitalizeStalker();

        NextPageSniffer::FunAfterLootRecall fnAfterLootRecall = [&]( size_t id, const std::string& szHref, const std::string& szStoragePath ){
            std::string szTime = Pinecone::DateTime().toString();
            this->mpParentHeistum->trace( { id, szHref, szTime, "DONE" } );
            Pinecone::Atom jo;
            jo[ "id"   ] = (int64_t) id;
            jo[ "href" ] = szHref;
            jo[ "time" ] = szTime;
            this->mnLastStalkerId = id;
            this->mpParentHeistum->tombInfo( jo );
        };

        NextPageSniffer::FunCompressRecall fnCompressRecall = [&]( size_t id, std::string& szHtml, const Pinecone::HTMLDocument* lpDocument ){
            this->compressSoloArchive( szHtml, lpDocument );
        };

        this->mpIndexSniffer->apply( &fnAfterLootRecall );
        this->mpIndexSniffer->apply( &fnCompressRecall );
        this->mpIndexSniffer->toSniffer();
    }

    void JsonIndexReaver::toEmbezzle() const {

    }

    void JsonIndexReaver::isTimeToFeast() const {
        this->MegaDOMIndexReaver::isTimeToFeast();
    }

}