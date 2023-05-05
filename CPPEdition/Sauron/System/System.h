#ifndef NONARON_SYSTEM_H
#define NONARON_SYSTEM_H
#include <Pinecone/Framework/defined.inc.h>
#include <Pinecone/Framework/Util/Tracer/Tracer.h>
#include <Sauron/System/defined.inc.h>
#include <Sauron/System/MeeseeksBox.h>
#include <string>
#include <fstream>
#include <mutex>

namespace Pinecone {
    class Atom;
}

namespace Sauron {
    _PINE_INTERFACE Mission;

    _PINE_INTERFACE Task;

    class CliqueAffairs;
    class PathRouter;

    class _PINE_API_OUT System {
    public:
        struct StartupCommand {
            int    argc;
            char** argv;
        };

    public:
        typedef Pinecone::Atom                            PineAtom    ;

    private:
        std::string                     mszMinionName             ;
        std::string                     mszServiceID              ;

    private:
        bool                            mbConsoleTrace            ;
        std::string                     mszInfoOut                ;
        std::string                     mszErrOut                 ;
        mutable std::ofstream           mInfoOut                  ;
        mutable std::ofstream           mErrOut                   ;
        Pinecone::Tracer                mConsoleEcho              ;
        Pinecone::Tracer                mInfoEcho                 ;
        Pinecone::Tracer                mErrEcho                  ;

        mutable std::mutex              mMutexTrace               ;

    private:
        StartupCommand                  mStartupCommand           ;
        std::string                     mszWorkingPath            ;
        PineAtom*                       mpGlobalConfig  = nullptr ;
        PineAtom*                       mpSystemConfig  = nullptr ;

    private:
        CliqueAffairs*                  mpCliqueAffairs           ;
        PathRouter*                     mpPathRouter              ;
        MeeseeksBox*                    mpMeeseeksBox             ;

    public:
        System() ;

        System( int argc, char *argv[] ) ;

        ~System();

    private:
        void handleStartupCommand( int argc, char *argv[] ) ;

        void init() ;

        void loadConfig();

        void loadTracer();

        void loadModule();

        void traceWelcomeInfo() const ;

        void reinterpretConfSystemNode();

    public:
        const StartupCommand* getStartupCommand() _PINE_SIMPLE_GETTER {
            return &this->mStartupCommand;
        }

        const std::string& getWorkingPath() const _PINE_NOEXCEPT {
            return this->mszWorkingPath;
        }

        const std::string& getServiceID() const _PINE_NOEXCEPT {
            return this->mszServiceID;
        }

        PineAtom*    getGlobalConfig() _PINE_SIMPLE_GETTER {
            return this->mpGlobalConfig;
        }

        PineAtom*    getSystemConfig() _PINE_SIMPLE_GETTER {
            return this->mpSystemConfig;
        }

        PineAtom*    queryDefaultBrowserSimConfig() _PINE_SIMPLE_GETTER ;

        const MeeseeksBox* getMeeseeksBox() _PINE_SIMPLE_GETTER {
            return this->mpMeeseeksBox;
        }

        const CliqueAffairs* getCliqueAffairs() _PINE_SIMPLE_GETTER {
            return this->mpCliqueAffairs;
        }

        const PathRouter* getPathRouter() _PINE_SIMPLE_GETTER {
            return this->mpPathRouter;
        }

    public:
        const Pinecone::Tracer& console() _PINE_SIMPLE_GETTER {
            return this->mConsoleEcho;
        }

        const Pinecone::Tracer& infocer() _PINE_SIMPLE_GETTER {
            return this->mInfoEcho;
        }

        const Pinecone::Tracer& errorcer() _PINE_SIMPLE_GETTER {
            return this->mErrEcho;
        }

    private:
        template<typename T >
        const Sauron::System& log0 ( const T& Anything                 ) const {
            this->mMutexTrace.lock();
            if( this->mbConsoleTrace ) {
                this->console().println( Anything );
            }
            //this->infocer().println( Anything );
            //this->mInfoOut.flush();
            this->mMutexTrace.unlock();
            return *this;
        }

        template<typename T >
        const Sauron::System& print0 ( const T& Anything                 ) const {
            this->mMutexTrace.lock();
            if( this->mbConsoleTrace ) {
                this->console().print( Anything );
            }
            //this->infocer().println( Anything );
            //this->mInfoOut.flush();
            this->mMutexTrace.unlock();
            return *this;
        }

    public:
        template<typename T >
        const Sauron::System& log ( const T& Anything                 ) const {
            return this->log0( Anything );
        }

        template<typename T, typename... Args >
        const Sauron::System& log ( const T& Anything, Args&&... args ) const {
            this->log0( Anything );
            this->log( args... );
            return *this;
        }

        template<typename T >
        const Sauron::System& print ( const T& Anything                 ) const {
            return this->print0( Anything );
        }

        template<typename T, typename... Args >
        const Sauron::System& print ( const T& Anything, Args&&... args ) const {
            this->print0( Anything );
            this->print( args... );
            return *this;
        }

        void beforeTraceLifecycleDetails () const {
            this->mMutexTrace.lock();
            this->console().print( "[System::LifecycleTrace] " );
            this->mMutexTrace.unlock();
        }

        template<typename... Args >
        const Sauron::System& traceLifecycleDetails ( Args&&... args ) const {
            this->beforeTraceLifecycleDetails();
            this->log( args... );
            return *this;
        }

    protected:
        void vitalizeSysProc();

        void vitalizeDaemon();

    public:
        int vitalize() ;

        int vitalize( const std::function<int32_t()>& fnInlet ) ;
    };
}


#endif //NONARON_SYSTEM_H
