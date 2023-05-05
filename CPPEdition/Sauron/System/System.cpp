#include <Pinecone/Framework/Element/Atom/Atom.h>
#include <Pinecone/Framework/Util/JSON/JSON.h>
#include <Pinecone/Framework/Util/File/FileUtils.h>
#include <Pinecone/Extension/System/IO/FileSystem.h>
#include <Sauron/System/MeeseeksBox.h>
#include <Sauron/System/Meeseek.h>
#include <Sauron/Heists/Period/NewsChronicle/NewsReaver.h>
#include <Sauron/Heists/Templated/Pinterest/PinterestReaver.h>
#include <Pinecone/Framework/Unit/Map/HashMap.h>
#include <Sauron/Heists/Templated/Wiki/WikiHeist.h>
#include <Sauron/Heists/Templated/IMDB/IMDBHeist.h>
#include <Sauron/Heists/Templated/DouBan/DouBanReaver.h>
#include <Sauron/Utils/StringHelper.h>
#include <Pinecone/Framework/Util/CSS/DOMTreeSelector.h>
#include <Sauron/Utils/BrowserSim.h>
#include <Sauron/Utils/Semantic/DragronExpressionParser.h>
#include <Pinecone/Framework/Util/File/File.h>
#include <Sauron/System/Clique/CliqueAffairs.h>
#include <Sauron/System/PathRouter.h>
#include <Sauron/Heistron/Edge/EdgeHeistKingServer.h>
#include <Sauron/Heistron/Edge/EdgeHeistClient.h>
#include "System.h"
#include "SysDaemon.h"
#include <Pinecone/Extension/Util/Net/Socket.h>

namespace Sauron {
    System::System() {
        this->init();
    }

    System::System( int argc, char *argv[] ) {
        this->handleStartupCommand( argc, argv );
    }

    System::~System() {
        delete this->mpGlobalConfig;
        delete this->mpMeeseeksBox;
        delete this->mpCliqueAffairs;
        delete this->mpPathRouter;
    }

    void System::handleStartupCommand( int argc, char* argv[] ) {
        this->init();
        this->mStartupCommand.argc = argc;
        this->mStartupCommand.argv = argv;
    }

    void System::init() {
        this->mszWorkingPath = Pinecone::FileSystem::myDir() + "\\";
        if( this->mszWorkingPath.find( "E:\\MyFiles\\" ) != std::string::npos ) {
            this->mszWorkingPath = R"(\\B-ServerKingpin\ARBOmnium\EnderChest\ARBOmnium\Sauron\)";
        }

        this->loadConfig();
        //this->loadTracer();
        this->loadModule();
        this->traceWelcomeInfo();
    }

    void System::loadConfig() {
        this->mpGlobalConfig    = new Pinecone::Atom;
        PineAtom& jConf         = *this->mpGlobalConfig;
        jConf                   = Pinecone::JSON::parse(
                Pinecone::FileUtils::readAll( this->mszWorkingPath + NONARON_MAIN_CONFIG_FILE )
        );


        this->reinterpretConfSystemNode();
        PineAtom& jSysConf      = *this->mpSystemConfig;
        this->mszMinionName     = jSysConf[ "MinionName" ].toString();
        this->mszServiceID      = jSysConf[ "ServiceID"  ].toString();

        PineAtom& jTracerConf   = jSysConf[ "Tracer" ];
        this->mbConsoleTrace    = (bool) jTracerConf[ "ConsoleTrace" ].getRawInt64Zone();
        this->mszInfoOut        = jTracerConf[ "InfoTracer" ].toString();
        this->mszErrOut         = jTracerConf[ "ErrTracer"  ].toString();

        std::string szPID       = Pinecone::StdString::toString( getpid() );
        this->mszInfoOut        = StringHelper::replaceAll( this->mszInfoOut , "${pid}", szPID );
        this->mszErrOut         = StringHelper::replaceAll( this->mszErrOut  , "${pid}", szPID );
    }

    void System::loadTracer() {
        this->mInfoOut.open( this->mszWorkingPath + this->mszInfoOut, std::ios::binary );
        this->mErrOut.open ( this->mszWorkingPath + this->mszErrOut, std::ios::binary );

        this->mInfoEcho.setOut( &this->mInfoOut );
        this->mErrEcho.setOut( &this->mErrOut );
    }

    void System::loadModule() {
        this->mpMeeseeksBox   = new MeeseeksBox( this );
    }

    void System::reinterpretConfSystemNode() {
        Pinecone::Atom& jSysConf    = ( *this->mpGlobalConfig )[ "System" ];
        this->mpSystemConfig        = &jSysConf;
        this->mpCliqueAffairs       = new CliqueAffairs( this );
        this->mpPathRouter          = new PathRouter( this );
    }

    void System::traceWelcomeInfo() const {
        std::string szPrimeDirective = Pinecone::StdString::toLowerCase(
                this->mpMeeseeksBox->getPrimeDirective()
        );

        this->log( "---------------------------------------------------------------" );
        this->log( "Greeting! My name is Sauron, I insight omniscience." );
        this->log( "Salve! Nomen Sauron est, omnia perspicio." );
        this->log( "----------------------System Information-----------------------" );
        this->log( "ProgramName: " NONARON_PROGRAM_NAME   );
        this->log( "Version    : " NONARON_VERSION        );
        this->log( "ReleaseDate: " NONARON_RELEASE_DATE   );
        this->log( "ServiceID  : " + this->mszServiceID   );
        this->log( "---------------------------------------------------------------" );
        this->log( "Minion " + this->mszMinionName + ", state your prime directive." );
        this->log( this->mszMinionName + ": To " + szPrimeDirective + " !" );
        this->log( "---------------------------------------------------------------" );
    }

    Pinecone::Atom *System::queryDefaultBrowserSimConfig() const _PINE_NOEXCEPT {
        PineAtom& joBrowserSimPolicies = (*this->mpSystemConfig)[ "BrowserSimPolicies" ];
        std::string szDefaultPolicy = joBrowserSimPolicies[ "DefaultPolicy" ].toString();
        return &joBrowserSimPolicies[ "PolicyGroup" ][ szDefaultPolicy ];
    }

    void System::vitalizeSysProc() {
        this->vitalizeDaemon();
    }

    void System::vitalizeDaemon() {
        bool bEnableDaemon = false;
        const System::StartupCommand* cmd = &this->mStartupCommand;
        if( cmd->argc > 1 ) {
            const char* lpszHeistCmd  = cmd->argv[ 1 ];
            bEnableDaemon = std::strcmp( lpszHeistCmd, "--daemon" ) == 0;
        }

        if( bEnableDaemon ) {
            SysDaemon daemon( this );
            daemon.keepHostAlive();
        }
    }

    int System::vitalize() {
        MeeseeksBox box( this );
        box.toExert();



//        InnerBrowserSim browserSim( this );
//        echo browserSim.getHTTPFile( "https://www.azlyrics.com/" );

//        Sauron::NewsReaver newsReaver( this );
//        newsReaver.activateHeist();

//        Sauron::YiXueComReaver yiXueComReaver( this->mpGlobalConfig );
//        yiXueComReaver.isTimeToFeast();


//        Sauron::PinterestReaver pinterestReaver;
//        pinterestReaver.isTimeToFeast();




//        Sauron::WikipediaHeist wikipediaHeist(this );
//        wikipediaHeist.toHeist();


//        std::string szPath = R"(\\B-SERVERKINGPIN\ARBOmnium\EnderChest\Facility\Wiki\Pages\300W_400W\)";
//        Pinecone::Directory idxDirectory( szPath );
//        std::vector<std::string > arrPaths = idxDirectory.toArray();
//        Pinecone::HashMap<std::string, bool > hashMap;
//        for ( int i = 0; i < arrPaths.size(); ++i ) {
//            hashMap[ arrPaths[i] ] = true;
//        }
//        echo hashMap.size();
//
//        for ( int i = 3000000; i < 3999999; ++i ) {
//            std::string szF = "page_" + Pinecone::StdString::toString(i) + ".html";
//            if( !hashMap.hasOwnProperty( szF ) ) {
//                echo i;
//            }
//        }

        return 2030;
    }

    int System::vitalize( const std::function<int32_t()> &fnInlet ) {
        this->vitalizeSysProc();

        int32_t nRet = fnInlet();
        if( nRet < 0 ) {
            return nRet;
        }
        return this->vitalize();
    }
}

