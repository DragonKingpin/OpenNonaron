#include <Pinecone/Framework/Element/Atom/Atom.h>
#include <Pinecone/Framework/Unit/Dictron/Dictron.h>
#include<Sauron/System/Meeseek.h>
#include <Sauron/System/MeeseeksBox.h>
#include "Heistron.h"
#include <Sauron/System/System.h>
#include <Pinecone/Framework/Util/File/FileUtils.h>
#include <Pinecone/Framework/Unit/String/Stereotype/CharStringStereotype.h>
#include <Sauron/Utils/ConfigHelper.h>
#include <Sauron/Heistron/Edge/EdgeHeistKingServer.h>
#include <Sauron/Heistron/Edge/EdgeHeistProtocol.h>
#include <Pinecone/Extension/Util/Net/SocketException.h>
#include "Heist.h"

namespace Sauron {
    HeistumSummoner HeistumSummoner::gSingletonSummoner;

    void HeistumSummoner::registerGenerator( const std::string &szClassName, FunHeistumInit &&generator ) {
        this->mMagicMirror[ szClassName ] = generator;
    }

    Heist* HeistumSummoner::newInstance ( const std::string &szClassName, Heistron* lpHeistron ) {
        mirror_type::pair_type* pKV = this->mMagicMirror.getKeyValueByKey( szClassName );
        if( pKV ){
            return pKV->Value( lpHeistron );
        }
        return nullptr;
    }

    Heistron::~Heistron() {
        delete this->mpEdgeHeistKingpin;
        this->Heistron::meltdown();
    }

    Heistron::Heistron( const std::string &szName, MeeseeksBox *lpBox ) : Meeseek( szName, lpBox ) {
        this->Heistron::loadConfig();
        this->traceInitInfo();
    }

    Pinecone::Atom *Heistron::queryHeistPrivateConfig( const Heistum *lpThat ) const {
        return &(*this->mpHeistList)[ lpThat->heistName() ];
    }

    Heist *Heistron::contriveByScheme ( const std::string &szSchemeName ) {
        if( this->mpHeistList->hasOwnProperty( szSchemeName ) ) {
            std::string szHeistumClassName = szSchemeName + this->missionName();

            Summoner* lpSummoner = Summoner::instance();
            if( lpSummoner->getMagicMirror().hasOwnProperty( szHeistumClassName ) ) {
                return lpSummoner->newInstance( szHeistumClassName, this );
            }
            throw Pinecone::IllegalArgumentException( "IllegalArgumentException: [Heistron] Fantasy scheme with no crew member." );
        }

        throw Pinecone::IllegalArgumentException( "IllegalArgumentException: [Heistron] No such heist scheme." );
    }

    std::string Heistron::queryCmdDesignatedHeist() const {
        const System::StartupCommand* cmd = this->getParentSystem()->getStartupCommand();
        if( this->mbEnableCmdCall && cmd->argc > 1 ) {
            const char* lpszHeistCmd  = cmd->argv[ 1 ];
            const char* lpszHeistName = Pinecone::BaseCharStringStereotype::tokenCursorSearch( lpszHeistCmd, "--heist=",false );
            if( lpszHeistName ) {
                return lpszHeistName;
            }
        }
        return "";
    }

    Heist *Heistron::queryGuerrillaHeist( const std::string& szSchemeName ) const _PINE_NOEXCEPT {
        pool_type::T_KeyValue* kv = this->mHeistsRoom.getKeyValueByKey( szSchemeName );
        if( kv ) {
            return kv->Value;
        }
        return nullptr;
    }

    void Heistron::afterRequestAccept( EdgeHeistProtocol* lpProtocol ) {
        EdgeHeistProtocol heistProtocol = *lpProtocol;
        Pinecone::Atom joHead;
        heistProtocol.accept( &joHead );
        if( joHead.hasOwnProperty( "HeistName" ) ) {
            std::string szHeistName = joHead[ "HeistName" ].toString();
            Heist* lpHeist = this->queryGuerrillaHeist( szHeistName );
            lpHeist->afterEdgeRequestAccept( &joHead, &heistProtocol );
        }
        else {
            throw Pinecone::SocketException( "SocketException: [EdgeHeistProtocol] Illegal HeistProtocol head, undefined HeistName." );
        }
    }


    void Heistron::extendDefaultTemplatedConfig() {
        PineAtom& jConf       = (*this->mpMeeseekConf)[ "TemplatedConfig" ];
        PineAtom& jSysDefault = *this->getParentSystem()->queryDefaultBrowserSimConfig();

        if( !jConf.hasOwnProperty( "BrowserSimConf" ) ) {
            jConf[ "BrowserSimConf" ] = jSysDefault;
        }
        else {
            PineAtom jo;
            ConfigHelper::overrideConfig( jo, jSysDefault, jConf[ "BrowserSimConf" ] );
            jConf[ "BrowserSimConf" ] = std::move( jo );
        }
    }

    void Heistron::loadConfig() {
        this->Meeseek::loadConfig();

        PineAtom& jConf = *this->mpMeeseekConf;
        std::string szHeistMode = jConf[ "HeistMode" ].toString();
        if( szHeistMode == "Designate" ) {
            this->mHeistMode = HeistMode::HM_DESIGNATE;
        }
        else if( szHeistMode == "SequentialMainline" ) {
            this->mHeistMode = HeistMode::HM_SEQUENTIAL_MAINLINE;
        }
        else if( szHeistMode == "SequentialPolling" ) {
            this->mHeistMode = HeistMode::HM_SEQUENTIAL_POLLING;
        }
        else if( szHeistMode == "ParallelMainline" ) {
            this->mHeistMode = HeistMode::HM_PARALLEL_MAINLINE;
        }
        else if( szHeistMode == "ParallelPolling" ) {
            this->mHeistMode = HeistMode::HM_PARALLEL_POLLING;
        }

        this->mszPrimeHeist     =  jConf[ "PrimeHeist"      ].toString();
        this->mpHeistList       = &jConf[ "Heists"          ];

        this->extendDefaultTemplatedConfig();
        this->reinterpretConfHeistsNode();
        this->mpTemplatedConfig = &jConf[ "TemplatedConfig" ];

        this->mbEnableCmdCall   =  (bool) jConf[ "EnableCmdCall"   ].getRawInt64Zone();
        this->mpEdgeKingConf    = &jConf[ "EdgeHeistServer" ];
        this->mbEnableGuerrilla = (bool ) (*this->mpEdgeKingConf)[ "EnableGuerrilla" ].getRawInt64Zone();
    }

    void Heistron::reinterpretConfHeistsNode() {
        for ( auto& kv : *this->mpHeistList->affirmDictron()->affirmMaptron() ) {
            if( kv.Value.isString() ) {
                std::string szFN = kv.Value.toString();
                kv.Value.jsonDecode( Pinecone::FileUtils::readAll( this->getParentSystem()->getWorkingPath() + szFN ) );
            }
        }
    }

    void Heistron::meltdown() {
        this->mHeistsRoom.clear([&]( Heist* lpHeist ){
            lpHeist->meltdown();
            delete lpHeist;
        });
    }

    void Heistron::vitalizeSequentialHeist( const std::string& szSchemeName ) {
        Heist* lpHeist = this->contriveByScheme( szSchemeName );
        this->mHeistsRoom.emplace( szSchemeName, lpHeist );
        lpHeist->toHeist();
    }

    void Heistron::vitalizeParallelHeist( const std::string& szSchemeName ) {

    }

    void Heistron::vitalizeGuerrillaHeist() {
        Pinecone::Maptron& heistList = *this->mpHeistList->affirmDictron()->affirmMaptron();
        heistList.forEach([&]( int id, Pinecone::Maptron::T_KeyValue& kv ){
            Pinecone::Atom* lpEdgeGuerrillaHeist = kv.Value.affirmDictron()->getValueByKey( "EdgeGuerrillaHeist" );
            if( lpEdgeGuerrillaHeist ) {
                Pinecone::Atom* lpEnableGuerrilla = lpEdgeGuerrillaHeist->affirmDictron()->getValueByKey( "EnableGuerrilla" );
                if( lpEnableGuerrilla && lpEnableGuerrilla->getRawInt64Zone() ) {
                    Heist* lpHeist = this->contriveByScheme( kv.Key );
                    this->mHeistsRoom.emplace( kv.Key, lpHeist );
                }
            }
        });

        this->mpEdgeHeistKingpin = new EdgeHeistKingServer( this );

        this->mpEdgeHeistKingpin->start();
    }

    void Heistron::dispatch() {
        if( this->isGuerrillaMode() ) {
            this->vitalizeGuerrillaHeist();
        }
        else {
            std::string szDesignatedHeist = this->queryCmdDesignatedHeist();
            if( szDesignatedHeist.empty() ){
                szDesignatedHeist = this->mszPrimeHeist;
            }
            this->vitalizeSequentialHeist( szDesignatedHeist );
        }

        this->waitForAllFinished();
    }

    void Heistron::waitForAllFinished() {

    }

    void Heistron::toSeek() {
        if( this->mbTraceLifecycle ) {
            this->getParentSystem()->traceLifecycleDetails( "[" + this->className() + "]::toSeek <Can do !>" );
        }

        this->dispatch();
    }
}