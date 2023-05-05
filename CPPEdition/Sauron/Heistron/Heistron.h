#ifndef NONARON_HEISTRON_H
#define NONARON_HEISTRON_H
#include <Pinecone/Framework/defined.inc.h>
#include <Ulfhedinn/config.inc.h>
#include <Pinecone/Framework/Unit/Map/LinkedTreeMap.h>
#include <Sauron/System/defined.inc.h>
#include <Sauron/System/Meeseek.h>
#include "Heists.h"

namespace Sauron {
    class Heistron;

    _PINE_ABSTRACT_CLASS Heist;

    _PINE_INTERFACE Heistum;

    class System;

    class EdgeHeistKingServer;
    class EdgeHeistProtocol;

    class _PINE_API_OUT HeistumSummoner {
    public:
        typedef Heist* (*FunHeistumInit) ( Heistron* lpHeistron );

        typedef Pinecone::LinkedTreeMap<std::string, FunHeistumInit >        mirror_type ;

    private:
        mirror_type            mMagicMirror       ;

    private:
        static HeistumSummoner gSingletonSummoner ;

    public:
        HeistumSummoner() _PINE_NOEXCEPT = default;

    public:
        inline static HeistumSummoner* instance() {
            return &HeistumSummoner::gSingletonSummoner;
        }

    public:
        size_t getMirrorSize() const _PINE_NOEXCEPT {
            return this->mMagicMirror.size();
        }

        mirror_type& getMagicMirror() {
            return this->mMagicMirror;
        }

        const mirror_type& getMagicMirror() const {
            return this->mMagicMirror;
        }

        void registerGenerator ( const std::string &szClassName, FunHeistumInit && generator );

        Heist* newInstance   ( const std::string &szClassName, Heistron* lpHeistron );

    };

    template <typename T >
    class _PINE_API_OUT HeistumMagicWand {
    public:
        static Heist* heistumSpawner( Heistron* lpHeistron ) {
            return new T( lpHeistron );
        }

        explicit HeistumMagicWand( const std::string &szClassName ){
            HeistumSummoner::instance()->registerGenerator( szClassName, HeistumMagicWand::heistumSpawner );
        }
    };

    #define HEISTUM_REGISTER(CLASS_NAME) HeistumMagicWand<CLASS_NAME> G_RegisterMirrorAuth##CLASS_NAME( #CLASS_NAME );

    class Heistron: _PINE_IMPLEMENTS public Meeseek {
    public:
        typedef HeistumSummoner                                           Summoner;
        typedef Pinecone::LinkedTreeMap<std::string, Heist* >             pool_type;

    public:
        enum HeistMode {
            HM_DESIGNATE = 0x01,
            HM_SEQUENTIAL_MAINLINE,
            HM_SEQUENTIAL_POLLING,

            HM_PARALLEL_MAINLINE,
            HM_PARALLEL_POLLING
        };

    protected:
        typedef Heistron                   Self ;
        typedef Pinecone::int64           int64 ;

    protected:
        PineAtom*                     mpTemplatedConfig   ;

    protected:
        HeistMode                     mHeistMode          ;
        std::string                   mszPrimeHeist       ;
        PineAtom*                     mpHeistList         ;
        bool                          mbEnableCmdCall     ;

    protected:
        pool_type                     mHeistsRoom         ;

    protected:
        bool                          mbEnableGuerrilla = false;
        PineAtom*                     mpEdgeKingConf      ;
        EdgeHeistKingServer*          mpEdgeHeistKingpin  ;

    public:
        Heistron( const std::string& szName, MeeseeksBox* lpBox );

        ~Heistron() _PINE_OVERRIDE;

    protected:
        virtual void extendDefaultTemplatedConfig();

        void loadConfig() _PINE_OVERRIDE;

        void reinterpretConfHeistsNode() ;

    public:
        const std::string& primeHeist() const {
            return this->mszPrimeHeist;
        }

        PineAtom* getHeistList() _PINE_SIMPLE_GETTER {
            return this->mpHeistList;
        };

        Pinecone::Atom* queryHeistPrivateConfig( const Heistum* lpThat ) const ;

        Pinecone::Atom* getTemplatedConfig() const {
            return this->mpTemplatedConfig;
        }

    public:
        std::string prototypeName() const _PINE_OVERRIDE {
            return this->className();
        }

        std::string className() const _PINE_OVERRIDE {
            return "Heistron";
        }

    public:
        Heist* contriveByScheme( const std::string& szSchemeName ) ;

        std::string queryCmdDesignatedHeist() const;

    public:
        EdgeHeistKingServer* getEdgeHeistKingpin() const _PINE_NOEXCEPT {
            return this->mpEdgeHeistKingpin;
        }

        PineAtom* getEdgeHeistServerConf() const _PINE_NOEXCEPT {
            return this->mpEdgeKingConf;
        }

        bool isGuerrillaMode() const _PINE_NOEXCEPT {
            return this->mbEnableGuerrilla;
        }

        Heist* queryGuerrillaHeist( const std::string& szSchemeName ) const _PINE_NOEXCEPT;

        void afterRequestAccept( EdgeHeistProtocol* lpProtocol );

    protected:
        void vitalizeSequentialHeist( const std::string& szSchemeName );

        void vitalizeParallelHeist( const std::string& szSchemeName );

        void vitalizeGuerrillaHeist();

        void dispatch() ;

        void waitForAllFinished();

    public:
        void meltdown() _PINE_OVERRIDE ;

        void toSeek() _PINE_OVERRIDE ;

    };
}

#endif //NONARON_HEISTRON_H
