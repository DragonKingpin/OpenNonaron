#include <Pinecone/Framework/Element/Atom/Atom.h>
#include "System.h"
#include "MeeseeksBox.h"
#include <Sauron/Heistron/Heistron.h>

namespace Sauron {
    MeeseeksBox::MeeseeksBox( System *lpSystem ) : mpSystem( lpSystem ) {
        this->init();
    }

    MeeseeksBox::~MeeseeksBox() {
        this->meltdownAll();
    }

    void MeeseeksBox::init() {
        this->loadConfig();
    }

    void MeeseeksBox::loadConfig() {
        PineAtom& jConf            = *this->mpSystem->getGlobalConfig();
        PineAtom& jMission         = jConf   [ "Mission"  ];
        PineAtom& jMeeseeks        = jMission[ "Meeseeks" ];
        this->mpMissionConfig      = &jMission;
        this->mpMeeseeksList       = &jMeeseeks;

        this->mszPrimeDirective    = jMission   [ "PrimeDirective" ].toString();

        std::string szMissionMode  = jMission   [ "MissionMode"    ].toString();
        if( szMissionMode == "Designate" ) {
            this->mMissionMode = MissionMode::HM_DESIGNATE;
        }
        else if( szMissionMode == "Schedule" ) {
            this->mMissionMode = MissionMode::HM_SCHEDULE;
        }

    }

    void MeeseeksBox::meltdownAll() {
        this->mMeeseekPool.forEach([&]( int32_t id, pool_type::pair_type& kv ){
            this->free( kv.Value );
        });
    }

    Meeseek *MeeseeksBox::spawn( const std::string &szName ) {
        PineAtom& jMeeseeks       = *this->mpMeeseeksList;
        if( jMeeseeks.hasOwnProperty( szName ) ) {
            if( szName == "Heist" ) {
                return new Heistron( szName, this );
            }
        }

        throw Pinecone::IllegalArgumentException( "IllegalArgumentException: [MeeseeksBox] Undefined job can't be done." );
    }

    void MeeseeksBox::free( Meeseek*& that ) {
        that->meltdown();
        delete that;
    }

    void MeeseeksBox::free( const std::string &szName ) {
        pool_type::pair_type* pKV = this->mMeeseekPool.getKeyValueByKey( szName );
        if( pKV ) {
            this->free( pKV->Value );
            this->mMeeseekPool.erase( szName );
        }
    }

    void MeeseeksBox::vitalizeDetachedMeeseek() {

    }

    void MeeseeksBox::vitalizePrimeMeeseek() {
        this->mpPrimeMeeseek = this->spawn( this->mszPrimeDirective );
        this->mpPrimeMeeseek->toSeek();
    }

    void MeeseeksBox::dispatch() {
        if( this->mMissionMode == MissionMode::HM_SCHEDULE ) {
            this->vitalizeDetachedMeeseek();
        }

        this->vitalizePrimeMeeseek();
    }

    void MeeseeksBox::waitForAllSought() {

    }

    void MeeseeksBox::toExert() {
        this->dispatch();

        this->waitForAllSought();
    }
}