#include <Sauron/Heistron/Edge/EdgeHeistKingServer.h>
#include <Sauron/Heistron/Edge/EdgeHeistClient.h>
#include <shlwapi.h>
#include <Pinecone/Framework/Unit/String/StdString.h>
#include <Pinecone/Framework/System/Exception/IllegalStateException.h>
#include <Pinecone/Framework/Util/File/FileUtils.h>
#include <Pinecone/Framework/Util/Random/Random.h>
#include <Sauron/Utils/IndexSniffer/IndexSniffer.h>
#include <Sauron/Utils/BrowserSim.h>
#include <Sauron/System/System.h>
#include "Heistron.h"
#include "Reavers.h"
#include "Reaver.h"
#include "Heist.h"

namespace Sauron {
    void Reaver::validateSpoil( const std::string &sz ) const {
        if( !Pinecone::StdString::endsWith( sz, "</html>" ) ) {
            throw Pinecone::IllegalStateException( "NetworkError::IncompleteFiles." );
        }
    }


    MegaDOMIndexReaver::MegaDOMIndexReaver( Heist *lpDispatcher ) : mpParentHeistum( lpDispatcher ), mnLootFrom( 0 ), mnLootTo( 0 ) {
        this->mpConfig          = this->mpParentHeistum->mpConfig;

        this->mnFragBase        = this->mpParentHeistum->mnFragBase;
        this->mnFragRange       = this->mpParentHeistum->mnFragRange;
        this->mszHeistHref      = this->mpParentHeistum->queryConfig( "HeistHref" )->toString();
        this->mszIndexPath      = this->mpParentHeistum->mszIndexPath;
        this->mszSpoilPath      = this->mpParentHeistum->mszSpoilPath;


        this->mFailureConf.lpjFailureConf      = (Pinecone::Atom*) this->mpParentHeistum->queryConfig( "FailureConf" );
        this->mFailureConf.nFailedFileSize     = (*this->mFailureConf.lpjFailureConf)[ "FailedFileSize"   ].getRawInt64Zone();
        this->mFailureConf.nFileRetrieveTime   = (*this->mFailureConf.lpjFailureConf)[ "FileRetrieveTime" ].getRawInt64Zone();

        if( !this->mpBrowserSim ){
            this->mpBrowserSim = new InnerBrowserSim( this );
        }

        Pinecone::Atom& joGuerrilla  = *(Pinecone::Atom*)this->mpParentHeistum->queryConfig( "EdgeGuerrillaHeist" );
        this->mszKingpinHost         = joGuerrilla[ "KingpinHost" ].toString();
        this->mnKingpinPort          = (int16_t ) joGuerrilla[ "KingpinPort" ].getRawInt64Zone();
        this->mbEnableGuerrilla      = (bool ) joGuerrilla[ "EnableGuerrilla" ].getRawInt64Zone();
        if( joGuerrilla.hasOwnProperty( "HeistClientID" ) ) {
            this->mszEdgeClientID    = joGuerrilla[ "HeistClientID" ].toString();
        }
        else {
            this->mszEdgeClientID    = this->parentHeistum()->getParentSystem()->getServiceID();
        }
    }

    MegaDOMIndexReaver::MegaDOMIndexReaver(
            Heist *lpDispatcher, size_t nLootFrom, size_t nLootTo
    ) : MegaDOMIndexReaver( lpDispatcher ) {
        this->mnLootFrom        = nLootFrom;
        this->mnLootTo          = nLootTo;
    }

    MegaDOMIndexReaver::~MegaDOMIndexReaver() {
        delete this->mpIndexSniffer;
        delete this->mpBrowserSim;
        delete this->mpEdgeClient;
    }

    void MegaDOMIndexReaver::validateSpoil( const std::string &sz ) const {
        if( sz.length() < this->mFailureConf.nFailedFileSize ) {
            throw Pinecone::IllegalStateException( "CompromisedFilesSize." );
        }
        if( sz.find( "</html>", sz.length() - 50 ) == std::string::npos ) {
            throw Pinecone::IllegalStateException( "NetworkError::IncompleteFiles." );
        }
    }

    Pinecone::Atom *MegaDOMIndexReaver::queryBrowserConfig() const {
        const Pinecone::Atom* lpBrowserSimConf = this->mpParentHeistum->queryConfig( "BrowserSimConf" );
        if( !lpBrowserSimConf ) {
            lpBrowserSimConf = this->mpParentHeistum->getParentSystem()->queryDefaultBrowserSimConfig();
        }
        return (Pinecone::Atom *)lpBrowserSimConf;
    }

    std::string MegaDOMIndexReaver::getHTTPFileFromHref( const std::string &szHref ) const {
        std::string szHtml;
        Pinecone::int64 nRetry = 0;
        Pinecone::IllegalStateException *lpLastError = nullptr;
        for ( size_t i = 0; i < this->mFailureConf.nFileRetrieveTime; ++i ) {
            try {
                szHtml   = this->getHTTPFile( szHref );
                this->validateSpoil( szHtml );
                break;
            }
            catch ( Pinecone::IllegalStateException& e ) {
                ++nRetry;
                lpLastError = &e;
            }
        }

        if( nRetry >= this->mFailureConf.nFileRetrieveTime - 1 && lpLastError ) {
            throw Pinecone::IllegalStateException( "IrredeemableLoot" );
        }

        return szHtml;
    }

    void MegaDOMIndexReaver::storeHrefCache( const std::string& szStoragePath, const std::string &szHref ) const {
        std::string szFileCache ;

        try {
            std::ifstream in( szStoragePath, std::ios::binary );
            if( in.is_open() ){
                szFileCache = Pinecone::FileUtils::readAll( in );
                this->validateSpoil( szFileCache );
                throw LootRecoveredException();
            }
        }
        catch ( LootRecoveredException& e ) {
            throw e;
        }
        catch ( ... ) {

        }

        szFileCache   = this->getHTTPFileFromHref( szHref );

        std::ofstream ofs( szStoragePath, std::ios::binary );
        if( ofs.good() ) {
            this->compressSoloArchive( szFileCache, nullptr );
            ofs << szFileCache;
            return;
        }

        throw Pinecone::IllegalStateException( "SpoilStorageFailed" );
    }

    void MegaDOMIndexReaver::lootById( size_t id ) const {
        std::string szStorageDir  = this->querySpoilStorageDir( id );
        std::string szStoragePath = this->querySpoilStoragePath( id );

        if( !PathIsDirectoryA( szStorageDir.c_str() ) ){
            CreateDirectoryA( szStorageDir.c_str(), nullptr );
        }

        std::string szDummyHref = this->queryHrefById( id );
        std::string szHref                                 ;
        if( Pinecone::StdString::startsWith( szDummyHref, "http", 0 ) ) {
            szHref = szDummyHref;
        }
        else {
            szHref = this->mszHeistHref + szDummyHref;
        }

        this->storeHrefCache( szStoragePath, szHref );
    }


    void MegaDOMIndexReaver::revitalizeStalker() const {

    }

    Heists::Metier MegaDOMIndexReaver::getMetier() const _PINE_NOEXCEPT {
        return this->mpParentHeistum->getMetier();
    }

    void MegaDOMIndexReaver::lootFromRange() const {
        for ( size_t i = this->mnLootFrom; i < this->mnLootTo; ++i ) {
            try {
                this->lootById( i );
                this->mpParentHeistum->noticeLootDone( this, i, false );
            }
            catch ( LootAbortException& e ) {
                this->mpParentHeistum->trace( { "PageID:", i, "Aborted" } );
            }
            catch ( LootRecoveredException& e ) {
                this->mpParentHeistum->noticeLootDone( this, i, true );
            }
            catch ( Pinecone::IllegalStateException& e ) {
                this->mpParentHeistum->error( { i, e.getMessage() } );
            }
        }
    }

    void MegaDOMIndexReaver::toRavage() const {
        this->lootFromRange();
    }

    void MegaDOMIndexReaver::toStalk() const {
        this->revitalizeStalker();
    }

    void MegaDOMIndexReaver::toEmbezzle() const {

    }

    void MegaDOMIndexReaver::isTimeToFeast() const {
        switch ( this->getMetier() ) {
            case Heists::Metier::M_REAVER : {
                this->toRavage();
                break;
            }
            case Heists::Metier::M_STALKER : {
                this->toStalk();
                break;
            }
            case Heists::Metier::M_EMBEZZLER : {
                this->toEmbezzle();
                break;
            }
            default: {
                break;
            }
        }
    }




    std::string MegaDOMIndexReaver::getHTTPFile( const std::string &szHref ) const {
        this->beforeNextHttpLoot();

        std::string szHtml = this->mpBrowserSim->getHTTPFile( szHref );

        this->afterLastHttpLoot();
        return szHtml;
    }

    void MegaDOMIndexReaver::beforeNextHttpLoot() const {

    }

    void MegaDOMIndexReaver::afterLastHttpLoot() const {

    }


    bool MegaDOMIndexReaver::isEdgeKingpin() const _PINE_NOEXCEPT {
        return this->parentHeistum()->mpHeistron->isGuerrillaMode();
    }

    void MegaDOMIndexReaver::beforeSendEdgeRequest() const {
        if( !this->mpEdgeClient ){
            this->mpEdgeClient = new EdgeHeistClient( (MegaDOMIndexReaver*)this );
        }
        else {
            this->mpEdgeClient->clientInit();
        }
    }

    void MegaDOMIndexReaver::sendEdgeSpoil( Pinecone::Atom *lpHead, const std::string &szSpoil ) const {
        this->beforeSendEdgeRequest();
        ( *lpHead )[ "HeistName"     ] = this->parentHeistum()->heistName();
        ( *lpHead )[ "Method"        ] = "POST";
        ( *lpHead )[ "ContentLength" ] = (Pinecone::reg_t) szSpoil.length();
        std::string szTelegram = lpHead->toJSONString() + "\n";
        szTelegram.append( szSpoil );
        this->mpEdgeClient->send( szTelegram );
        this->mpEdgeClient->close();
    }

    void MegaDOMIndexReaver::queryEdgeRequest( Pinecone::Atom *lpHead, Pinecone::Atom *lpResponse ) const {
        this->beforeSendEdgeRequest();
        ( *lpHead )[ "HeistName"     ] = this->parentHeistum()->heistName();
        ( *lpHead )[ "Method"        ] = "Query";
        std::string szTelegram = lpHead->toJSONString() + "\n";
        this->mpEdgeClient->send( szTelegram );
        this->mpEdgeClient->accept( lpResponse );
        this->mpEdgeClient->close();
    }

    void MegaDOMIndexReaver::afterEdgeRequestAccept( Pinecone::Atom *lpHead, EdgeHeistProtocol *lpProtocol ) {

    }
}