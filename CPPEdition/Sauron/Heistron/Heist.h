#ifndef NSAURON_HEIST_H
#define NSAURON_HEIST_H
#include <fstream>
#include <Pinecone/Framework/defined.inc.h>
#include <Ulfhedinn/config.inc.h>
#include <Pinecone/Framework/Unit/List/ArrayList.h>
#include <Pinecone/Framework/Element/Aton/Aton.h>
#include <Pinecone/Framework/Element/Atom/Atom.h>
#include <Pinecone/Framework/Util/Tracer/Tracer.h>
#include <Sauron/System/TaskThreadDispatcher.h>
#include <Sauron/System/Prototype/Task.h>
#include "Heists.h"

namespace Sauron {
    class System;

    class Heistron;

    class MegaDOMIndexReaver;

    class EdgeHeistProtocol;

    _PINE_INTERFACE Heistum : _PINE_EXTENDS public Task {
        virtual std::string heistName() const = 0;

        std::string taskName() const _PINE_OVERRIDE {
            return this->heistName();
        }

        virtual void meltdown() = 0;

        virtual void toRavage() = 0;

        virtual void toStalk() = 0;

        virtual void toEmbezzle() = 0;

        virtual void toHeist() = 0;

        virtual void afterEdgeRequestAccept( Pinecone::Atom* lpHead, EdgeHeistProtocol* lpProtocol ) = 0;
    };

    _PINE_ABSTRACT_CLASS _PINE_API_OUT Heist : _PINE_EXTENDS public TaskThreadDispatcher, _PINE_IMPLEMENTS public Heistum {
    public:
        enum ExtendMode {
            EM_NO_EXTEND = 0x01, EM_INSTANCE, EN_POLLING
        };

    protected:
        typedef Heist                            Self     ;
        typedef Pinecone::int64                  int64    ;
        typedef Pinecone::Atom                   PineAtom ;
        typedef Heists::HeistType                HeistType;
        typedef Heists::Metier                   Metier   ;

    protected:
        bool                          mbFromDeathPoint    ;
        bool                          mbRevitalized       ;
        mutable size_t                mnLastMinDeathPoint ;
        Metier                        mMetier             ;

    public:
        HeistType                     mHeistType          ;
        std::string                   mszIndexPath        ;
        std::string                   mszSpoilPath        ;
        size_t                        mnFragBase          ;
        size_t                        mnFragRange         ;
        std::string                   mszWorkingPath      ;
        bool                          mbMasterConfQuery   ;

    protected:
        std::string                   mszDyingMsgFile     ;
        std::string                   mszInfoOut          ;
        std::string                   mszErrOut           ;
        std::string                   mszSysOut           ;

        mutable std::ofstream         mInfoOut            ;
        mutable std::ofstream         mErrOut             ;
        mutable std::ofstream         mSysOut             ;

        Pinecone::Tracer              mConsoleEcho        ;
        Pinecone::Tracer              mInfoEcho           ;
        Pinecone::Tracer              mErrEcho            ;
        Pinecone::Tracer              mSysEcho            ;
        mutable std::mutex            mMutexTrace         ;
    public:
        std::string                   mszMutualIndexFile  ;
        Heistron*                     mpHeistron          ;
        Pinecone::Atom*               mpPrivateConfig     ;
        Pinecone::Atom*               mpInsPrivateConfig  ;
        Pinecone::Atom*               mpConfig            ;
        Pinecone::Atom                mjLocalReDict       ;
        Pinecone::Atom                mjIndex             ;

    protected:
        ExtendMode                    mExtendMode         = Self::ExtendMode::EM_NO_EXTEND ;
        std::string                   mszInstanceExtend   ;

    protected:
        bool                          mbEnableGuerrilla = false;

    protected:
        static std::string evalDatetime() ;

    public:
        explicit Heist( Heistron* lpHeistron );

        virtual ~Heist();

    protected:
        virtual void evalTaskCount();

        virtual void applyInstanceConfig();

        virtual void overrideRecursiveObjInstanceConfig( const std::string& szExtendName, const std::string& szThatObjName );

        virtual void overrideByInstanceConfig( const std::string& szExtendName );

        virtual void reinterpretConfig();

        virtual void queryRangeFromMaster();

        virtual void loadConfigFromMaster();

        virtual void loadConfig();

        virtual Pinecone::Atom* loadIndex();

        virtual void loadTracer();

        virtual void init();

    protected:
        void afterThreadAllocated () _PINE_OVERRIDE;

        size_t recordRavageDeathDiary() const ;

        void revitalizeRavageFromDeathPoint();

        virtual void vitalizeStalker() = 0;

    public:
        HeistType getHeistType() _PINE_SIMPLE_GETTER {
            return this->mHeistType;
        }

        bool isFromDeathPoint() _PINE_SIMPLE_GETTER {
            return this->mbFromDeathPoint;
        }

        std::string getDyingMsgFileName() const {
            return this->mszWorkingPath + this->mszDyingMsgFile;
        }

        bool isExtendInstanceMode() _PINE_SIMPLE_GETTER {
            return this->mExtendMode == Self::ExtendMode::EM_INSTANCE;
        }

        bool isExtendPollingMode() _PINE_SIMPLE_GETTER {
            return this->mExtendMode == Self::ExtendMode::EN_POLLING;
        }

        const Pinecone::Atom* queryConfig( const std::string& szKey ) const ;

        // Object[ szPrimeKey ][ szSubKey ] with overridden
        const Pinecone::Atom* queryConfig( const std::string& szPrimeKey, const std::string& szSubKey ) const ;

        std::string queryStringConfig( const std::string& szPrimeKey, const std::string& szDefault ) const ;

        std::string queryStringConfig( const std::string& szPrimeKey, const std::string& szSubKey, const std::string& szDefault ) const ;

        int64_t queryInt64Config( const std::string& szPrimeKey, const std::string& szSubKey, int64_t nDefault ) const ;

        Pinecone::Atom* getHeistConfig() _PINE_SIMPLE_GETTER {
            return this->mpConfig;
        }

        System* getParentSystem() _PINE_SIMPLE_GETTER;

        Heists::Metier getMetier() _PINE_SIMPLE_GETTER {
            return this->mMetier;
        }

        const std::string& getInstanceExtend() _PINE_SIMPLE_GETTER {
            return this->mszInstanceExtend;
        }

    private:
        void print0 ( const char* lpsz ) const ;

        void print0 ( const std::string& sz ) const ;

        void beforeTraceLifecycleDetails () const ;

        template<typename T >
        void traceLifecycleDetails0 ( const T& Anything                 ) const {
            this->print0( Anything );
        }

        template<typename T, typename... Args >
        void traceLifecycleDetails0 ( const T& Anything, Args&&... args ) const {
            this->print0( Anything );
            this->traceLifecycleDetails0( args... );
        }

    public:
        bool isTraceLifecycle() _PINE_SIMPLE_GETTER ;

        template<typename... Args >
        const Sauron::Heist& traceLifecycleDetails ( Args&&... args ) const {
            this->beforeTraceLifecycleDetails();
            this->traceLifecycleDetails0( args... );
            this->traceLifecycleDetails0( "\n" );
            return *this;
        }

    public:
        void trace     ( Pinecone::Aton::Liston&& info, const char* lpszEnd = "\n" ) const ;

        void error     ( Pinecone::Aton::Liston&& info ) const ;

        void sysInfo   ( Pinecone::Aton::Liston&& info, const char* lpszEnd = "\n" ) const ;

        void tombInfo  ( const Pinecone::Atom & info ) const ;

        Pinecone::Atom queryDefaultTomb() const ;

        void noticeLootDone( const MegaDOMIndexReaver* lpReaver, size_t id, bool bIsRecovered ) const;

    public:
        std::string prototypeName() const _PINE_OVERRIDE {
            return this->className();
        }

        std::string className() const _PINE_OVERRIDE {
            return "Heist";
        }

        std::string heistName() const _PINE_OVERRIDE = 0;

    public:
        void meltdown() _PINE_OVERRIDE;

    public:
        void toRavage() _PINE_OVERRIDE;

        void toStalk() _PINE_OVERRIDE;

        void toEmbezzle() _PINE_OVERRIDE;

        void toHeist() _PINE_OVERRIDE;

    public:
        bool isGuerrillaHeist() const _PINE_NOEXCEPT{
            return this->mbEnableGuerrilla;
        }

        void afterEdgeRequestAccept( Pinecone::Atom* lpHead, EdgeHeistProtocol* lpProtocol ) _PINE_OVERRIDE;
    };
}


#endif //NSAURON_HEIST_H
