#include <Sauron/System/System.h>
#include <Sauron/Utils/ConfigHelper.h>
#include <Pinecone/Framework/Util/File/FileUtils.h>
#include <Pinecone/Framework/Util/DateTime/DateTime.h>
#include <Sauron/System/PathRouter.h>
#include <Sauron/Utils/Semantic/DragronExpressionParser.h>
#include "Reaver.h"
#include "Heist.h"
#include "Heistron.h"

namespace Sauron {
    void Heist::evalTaskCount() {
        this->mnStepTask = this->mnTakeFrom;
        this->mnSumTasks = this->mnTakeTo - this->mnTakeFrom;
    }

    void Heist::applyInstanceConfig() {
        Pinecone::Atom &jHeistConf = *this->mpConfig;
        std::string szExtendMode;
        ConfigHelper::evalString( szExtendMode, "ExtendMode", jHeistConf );
        if( !szExtendMode.empty() ) {
            if( szExtendMode == "Instance" ) {
                this->mExtendMode        = Self::ExtendMode::EM_INSTANCE;
                this->mszInstanceExtend  = jHeistConf[ "InstanceExtend" ].toString();

                this->overrideByInstanceConfig( this->mszInstanceExtend );
            }
            else if ( szExtendMode == "Polling" ) {
                this->mExtendMode = Self::ExtendMode::EN_POLLING;
            }
            else {
                this->mExtendMode = Self::ExtendMode::EM_NO_EXTEND;
            }
        }
    }

    void Heist::overrideRecursiveObjInstanceConfig( const std::string &szExtendName, const std::string& szThatObjName ) {
        Pinecone::Atom &jParentConf  = *this->mpConfig;
        Pinecone::Atom &jExtendConf  = jParentConf[ "Extends" ][ szExtendName ];

        Pinecone::Atom joDummy;
        ConfigHelper::overrideConfig(
                joDummy, jParentConf[ szThatObjName ], jExtendConf[ szThatObjName ]
        );
        (*this->mpConfig) [ szThatObjName ] = std::move( joDummy );
    }

    void Heist::overrideByInstanceConfig( const std::string &szExtendName ) {
        Pinecone::Atom &jParentConf  = *this->mpConfig;
        Pinecone::Atom &jExtendConf  = jParentConf[ "Extends" ][ szExtendName ];
        if( !jExtendConf.isUndefined() ) {
            this->mpInsPrivateConfig   = &jExtendConf;
            this->mpConfig             = &jExtendConf[ "__OVERRIDDEN_CONFIG__" ];
            ConfigHelper::overrideConfig(
                    *this->mpConfig, jParentConf, jExtendConf, false
            );
            // Due to this context is complex, recursively override for object should forbid.

            this->overrideRecursiveObjInstanceConfig( szExtendName, "FailureConf" );

            Pinecone::Atom& jFinalIndexSniffer  = (*this->mpConfig)[ "__FinalIndexSnifferConf" ];
            ConfigHelper::overrideConfig(
                    jFinalIndexSniffer, jParentConf[ "IndexSniffer" ], jExtendConf[ "IndexSniffer" ], false
            );
        }
    }

    void Heist::reinterpretConfig() {
        this->mjLocalReDict = *this->getParentSystem()->getPathRouter()->getReTabDictionary();
        this->mjLocalReDict[ "HeistName"     ] = this->heistName();
        this->mjLocalReDict[ "ClassName"     ] = this->className();
        this->mjLocalReDict[ "PrototypeName" ] = this->prototypeName();

        PineAtom& joFinalConf = (*this->mpPrivateConfig)[ "__REINTERPRETED_OVERRIDDEN_CONFIG__" ];

        this->mjLocalReDict[ "this" ].referenceAssign( this->mpConfig );
        auto fnReinterpretObj = [&]( PineAtom& joNeoObj, PineAtom& joLegacy ) {
            for ( auto & kv : *joLegacy.affirmDictron()->affirmMaptron() ) {
                if( kv.Key.front() != '-' ) {
                    PineAtom& jNeo = joNeoObj[ kv.Key ];
                    if( kv.Value.isString() ){
                        std::string szRaw = kv.Value.toString();
                        DragronExpressionParser parser( &this->mjLocalReDict, szRaw.c_str() );
                        jNeo = parser.eval();
                    }
                    else {
                        jNeo = kv.Value;
                    }

                    this->mjLocalReDict[ kv.Key ].referenceAssign( jNeo );
                }
                else {
                    joNeoObj[ kv.Key ] = kv.Value;
                }
            }
        };
        fnReinterpretObj( this->mjLocalReDict[ "this" ], *this->mpConfig ); // Reinterpret recursive eval. e.g. 'PreK: ${K1} K:XXX, K1:${K}'
        fnReinterpretObj( joFinalConf, *this->mpConfig );
        this->mjLocalReDict.affirmDictron()->erase( "this" );

        this->mpConfig = &joFinalConf;
    }

    void Heist::queryRangeFromMaster() {
        //TODO
    }

    void Heist::loadConfigFromMaster() {
        if( this->mbMasterConfQuery ) {
            this->queryRangeFromMaster();
        }
    }

    void Heist::loadConfig() {
        this->mpPrivateConfig      = this->mpHeistron->queryHeistPrivateConfig( this );
        this->mpConfig             = &(*this->mpPrivateConfig)[ "__OVERRIDDEN_CONFIG__" ];
        ConfigHelper::overrideConfig( *this->mpConfig, *this->mpHeistron->getTemplatedConfig(), *this->mpPrivateConfig );

        this->applyInstanceConfig();
        this->reinterpretConfig();
        Pinecone::Atom &jHeistConf = *this->mpConfig;

        this->mHeistType            = Heists::queryHeistType( jHeistConf[ "HeistType" ].toString() );
        this->mszIndexPath          = jHeistConf[ "IndexPath"    ].toString();
        this->mszSpoilPath          = jHeistConf[ "SpoilPath"    ].toString();
        this->mnFragBase            = (size_t) jHeistConf[ "FragBase"          ].getRawInt64Zone();
        this->mnFragRange           = (size_t) jHeistConf[ "FragRange"         ].getRawInt64Zone();

        this->mszWorkingPath        = jHeistConf[ "WorkingPath"  ].toString();
        this->mszDyingMsgFile       = jHeistConf[ "DyingMsgFile" ].toString();
        this->mszInfoOut            = jHeistConf[ "InfoTracer" ].toString();
        this->mszErrOut             = jHeistConf[ "ErrTracer"  ].toString();
        this->mszSysOut             = jHeistConf[ "SysTracer"  ].toString();

        this->mbMasterConfQuery     = (bool)   jHeistConf[ "MasterConfQuery"  ].getRawInt64Zone();
        this->mnTakeFrom            = (size_t) jHeistConf[ "TaskFrom"         ].getRawInt64Zone();
        this->mnTakeTo              = (size_t) jHeistConf[ "TaskTo"           ].getRawInt64Zone();
        this->mnMaximumThread       = (size_t) jHeistConf[ "MaximumThread"    ].getRawInt64Zone();
        this->mnEachTasksPerThread  = (size_t) jHeistConf[ "ReaverTasks"      ].getRawInt64Zone();
        this->mbFromDeathPoint      = (bool)   jHeistConf[ "FromDeathPoint"   ].getRawInt64Zone();
        this->mMetier               = Heists::queryMetier( jHeistConf[ "Metier" ].toString() );
        this->mszMutualIndexFile    = jHeistConf[ "MutualIndexFile" ].toString();
        this->evalTaskCount();

        this->mnLastMinDeathPoint   = this->mnTakeFrom;

        Pinecone::Atom& joGuerrilla = jHeistConf[ "EdgeGuerrillaHeist" ];
        this->mbEnableGuerrilla     = (bool ) joGuerrilla[ "EnableGuerrilla" ].getRawInt64Zone();

        this->loadConfigFromMaster();
    }

    void Heist::loadTracer() {
        Pinecone::Atom &jHeistConf  = *this->mpConfig;

        this->mInfoOut.open( this->mszWorkingPath + this->mszInfoOut, std::ios::binary );
        this->mErrOut.open ( this->mszWorkingPath + this->mszErrOut, std::ios::binary );
        this->mSysOut.open ( this->mszWorkingPath + this->mszSysOut, std::ios::binary );

        this->mInfoEcho.setOut( &this->mInfoOut );
        this->mErrEcho.setOut( &this->mErrOut );
        this->mSysEcho.setOut( &this->mSysOut );
    }

    void Heist::init() {
        this->loadConfig();

        switch ( this->mMetier ) {
            case Heists::Metier::M_REAVER : {
                this->revitalizeRavageFromDeathPoint();
                break;
            }
            case Heists::Metier::M_STALKER : {
                // TODO
                break;
            }
            case Heists::Metier::M_EMBEZZLER : {
                // TODO
                break;
            }
            default: {
                break;
            }
        }

        this->loadTracer();

        this->traceLifecycleDetails( "[Heistum<" + this->heistName() + ">] <A new heist has been contrived.>" );
    }

    const Pinecone::Atom* Heist::queryConfig( const std::string &szKey ) const {
        return this->mpConfig->getDictron()->getValueByKey( szKey );
    }

    const Pinecone::Atom *Heist::queryConfig( const std::string &szPrimeKey, const std::string& szSubKey ) const {
        const Pinecone::Atom* lpConf = this->queryConfig( szPrimeKey );
        if( lpConf ) {
            lpConf = lpConf->getDictron()->getValueByKey( szSubKey );
        }

        return lpConf;
    }

    std::string Heist::queryStringConfig( const std::string &szPrimeKey, const std::string &szDefault ) const {
        const PineAtom* lp = this->queryConfig( szPrimeKey );
        if( lp ) {
            return lp->toString();
        }
        return szDefault;
    }

    std::string Heist::queryStringConfig( const std::string &szPrimeKey, const std::string &szSubKey, const std::string &szDefault ) const {
        const PineAtom* lp = this->queryConfig( szPrimeKey, szSubKey );
        if( lp ) {
            return lp->toString();
        }
        return szDefault;
    }

    int64_t Heist::queryInt64Config( const std::string &szPrimeKey, const std::string &szSubKey, int64_t nDefault ) const {
        const PineAtom* lp = this->queryConfig( szPrimeKey, szSubKey );
        if( lp ) {
            return lp->getRawInt64Zone();
        }
        return nDefault;
    }

    System *Heist::getParentSystem() _PINE_SIMPLE_GETTER {
        return this->mpHeistron->getParentSystem();
    }

    void Heist::print0( const char *lpsz ) const {
        this->getParentSystem()->print( lpsz );
    }

    void Heist::print0( const std::string& sz ) const {
        this->getParentSystem()->print( sz );
    }

    void Heist::beforeTraceLifecycleDetails() const {
        this->getParentSystem()->beforeTraceLifecycleDetails();
    }

    bool Heist::isTraceLifecycle() _PINE_SIMPLE_GETTER {
        return this->mpHeistron->isTraceLifecycle();
    }

    Pinecone::Atom *Heist::loadIndex() {
        std::string szIndexPath = this->queryConfig( "IndexPath" )->toString();
        std::ifstream ifsIndex  ( szIndexPath + "/" + this->mszMutualIndexFile, std::ios::binary );
        if( ifsIndex.is_open() ) {
            this->mjIndex           = Pinecone::JSON::parse( Pinecone::FileUtils::readAll( ifsIndex ) );
        }
        return &this->mjIndex;
    }

    std::string Heist::evalDatetime() {
        return Pinecone::DateTime().toString();
    }

    Heist::Heist( Heistron* lpHeistron ) :
    TaskThreadDispatcher(), mpConfig( nullptr ),
    mnLastMinDeathPoint( 0 ), mbFromDeathPoint( false ), mbRevitalized( false ), mMetier( Heists::Metier::M_REAVER ) {
        this->mpHeistron  = lpHeistron;
    }

    Heist::~Heist() {
        this->Heist::meltdown();
    }

    void Heist::afterThreadAllocated() {
        switch ( this->mMetier ) {
            case Heists::Metier::M_REAVER : {
                this->recordRavageDeathDiary();
                break;
            }
            case Heists::Metier::M_STALKER : {
                // TODO
                break;
            }
            case Heists::Metier::M_EMBEZZLER : {
                // TODO
                break;
            }
            default: {
                break;
            }
        }
    }

    size_t Heist::recordRavageDeathDiary() const {
        size_t nMinTaskFrom = -1;
        for( auto& kv : this->mThreadPool ) {
            if( kv.Value->mnTaskFrom < nMinTaskFrom ){
                nMinTaskFrom = kv.Value->mnTaskFrom;
            }
        }
        this->mnLastMinDeathPoint = nMinTaskFrom;
        return this->mnLastMinDeathPoint;
    }

    void Heist::revitalizeRavageFromDeathPoint() {
        if( this->mbFromDeathPoint ) {
            Pinecone::Atom &jHeistConf  = *this->mpConfig;
            std::string szSysOut        = this->mszWorkingPath + jHeistConf[ "SysTracer"  ].toString();

            std::ifstream ifSys ( szSysOut, std::ios::binary );
            if ( ifSys.is_open() ){
                ifSys.seekg( -256, std::ios::end );
                std::string szLastLine;
                std::string szLine;
                while ( std::getline( ifSys, szLine ) ){
                    if ( szLine.length() > 2 ) {
                        szLastLine = std::move( szLine );
                    }
                }

                if( szLastLine.length() > 2 && ( szLastLine[0] == '{' || szLastLine[1] == '{' ) ){
                    while ( szLastLine.back() != '}' ){
                        szLastLine.pop_back();
                    }
                    Pinecone::Atom jDyingMsg;
                    try {
                        jDyingMsg.jsonDecode( szLastLine );
                        this->mnLastMinDeathPoint = (int64) jDyingMsg[ "LastMinDeathPoint" ].getRawInt64Zone();
                        this->mnTakeFrom = this->mnLastMinDeathPoint;
                        this->evalTaskCount();
                        this->mbRevitalized = true;
                    }
                    catch ( Pinecone::ParseException & e ) {
                        this->mbRevitalized = false;
                    }
                }
            }
        }
    }

    void Heist::trace     ( Pinecone::Aton::Liston&& info, const char* lpszEnd ) const {
        this->mMutexTrace.lock();
        for ( auto& item : info ) {
            this->mConsoleEcho, item;
            this->mInfoEcho, item;
        }
        this->mConsoleEcho, std::this_thread::get_id(), lpszEnd;
        this->mInfoEcho, std::this_thread::get_id(), lpszEnd;
        this->mInfoOut.flush();
        this->mMutexTrace.unlock();
    }

    void Heist::error     ( Pinecone::Aton::Liston&& info ) const {
        this->mMutexTrace.lock();
        for ( auto& item : info ) {
            this->mConsoleEcho, item;
            this->mErrEcho, item;
        }
        this->mConsoleEcho, std::this_thread::get_id(), endp;
        this->mErrEcho, std::this_thread::get_id(), endp;
        this->mErrOut.flush();
        this->mMutexTrace.unlock();
    }

    void Heist::sysInfo   ( Pinecone::Aton::Liston&& info, const char* lpszEnd ) const {
        this->mMutexTrace.lock();
        for ( auto& item : info ) {
            this->mSysEcho, item;
        }
        this->mSysEcho, std::this_thread::get_id(), lpszEnd;
        this->mSysOut.flush();
        this->mMutexTrace.unlock();
    }

    void Heist::tombInfo  ( const Pinecone::Atom & info ) const {
        this->mMutexTrace.lock();
        if( !this->mszDyingMsgFile.empty() ){
            std::ofstream ofDying( this->mszWorkingPath + this->mszDyingMsgFile, std::ios::binary );
            info.serialize( ofDying );
            ofDying.close();
        }
        this->mMutexTrace.unlock();
    }

    Pinecone::Atom Heist::queryDefaultTomb() const {
        std::string szDyingMsgFileName = this->getDyingMsgFileName();
        std::ifstream ifs( szDyingMsgFileName, std::ios::binary );
        Pinecone::Atom tomb;
        if( ifs.is_open() ) {
            std::string szJson = Pinecone::FileUtils::readAll( ifs );
            tomb.jsonDecode( szJson );
        }
        return tomb;
    }

    void Heist::noticeLootDone( const MegaDOMIndexReaver* lpReaver, size_t id, bool bIsRecovered ) const {
        char* buf = new char[ 512 ];
        const char* lpszState = "DONE";
        if( bIsRecovered ) {
            lpszState = "RECOVERED";
        }
        sprintf(
                buf, R"({"From":%lld, "To":%lld, "PageID":%lld, "State":"%s"})",
                (int64)lpReaver->getLootFrom(), (int64)lpReaver->getLootTo(), (int64)id, lpszState
        );
        this->trace( { buf } );


        sprintf(
                buf, R"({"Speed":%lld, "ReaverCrews":%lld, "Spoils":%lld ,"Time":"%s", "LastMinDeathPoint": %lld})",
                (int64)this->mnSpeed, (int64)this->mnCurrentTheadSum, (int64)this->mnMutualSum,
                Pinecone::DateTime().toString().c_str(), (int64) this->mnLastMinDeathPoint
        );
        this->sysInfo( { buf } );
        delete[] buf;

        this->mMutexTrace.lock();
        ++this->mnMutualSum;
        this->mMutexTrace.unlock();
    }

    void Heist::meltdown() {

    }

    void Heist::toRavage() {
        if( this->mbRevitalized ) {
            const char* lpInfo = "[Successfully revitalize from death point]";
            this->trace( { lpInfo } );
            this->sysInfo( { lpInfo } );
        }
        this->loadIndex();

        this->mnMaximumAllocate = this->mnSumTasks / this->mnEachTasksPerThread + 1;
        char* buf = new char[ 512 ];
        sprintf(
                buf, "{ From:%lld, To:%lld, BatchDesired:%lld, Tasks:%lld, Each:%lld, IdxSum:%lld }",
                (int64)this->mnTakeFrom, (int64)this->mnTakeTo, (int64)this->mnMaximumAllocate,
                (int64)this->mnSumTasks, (int64)this->mnEachTasksPerThread, (int64)this->mjIndex.size()
        );
        this->sysInfo( { buf } );
        delete[] buf;

        //std::ios::sync_with_stdio( false );
        this->polling();

        this->trace( { "[SuccessfullyHeist::Ravage]" } );
    }

    void Heist::toStalk() {
        this->vitalizeStalker();
    }

    void Heist::toEmbezzle() {

    }

    void Heist::toHeist() {
        if( this->isExtendInstanceMode() ) { // TODO: Polling Mode.
            this->traceLifecycleDetails(
                    "[Heistum<" + this->heistName() + ">::<" + this->mszInstanceExtend + ">]::toHeist"
            );
        }
        else if( this->mExtendMode == ExtendMode::EM_NO_EXTEND ) {
            this->traceLifecycleDetails(
                    "[Heistum<" + this->heistName() + ">]::toHeist"
            );
        }

        switch ( this->mMetier ) {
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

    void Heist::afterEdgeRequestAccept( Pinecone::Atom* lpHead, EdgeHeistProtocol* lpProtocol ) {

    }
}