#ifndef NONARON_REAVER_H
#define NONARON_REAVER_H
#include <string>
#include <Pinecone/Framework/defined.inc.h>
#include <Ulfhedinn/config.inc.h>
#include <Pinecone/Framework/Unit/String/StdString.h>
#include "Heists.h"

namespace Pinecone {
    class HTMLDocument;

    class Atom;
}

namespace Sauron {
    _PINE_ABSTRACT_CLASS Heist;
    _PINE_ABSTRACT_CLASS IndexSniffer;
    _PINE_ABSTRACT_CLASS BrowserSim;
    class EdgeHeistClient;
    class EdgeHeistProtocol;

    _PINE_INTERFACE Reaver {
        virtual std::string prototypeName() const = 0;

        virtual std::string className() const = 0;

        virtual std::string crewName() const = 0;

        virtual Heist* parentHeistum() const = 0;

        virtual void validateSpoil( const std::string& sz ) const ;

        virtual size_t getThresholdFailedFileSize() const  _PINE_NOEXCEPT {
            return 2000;
        }

        virtual void isTimeToFeast() const = 0;
    };

    _PINE_INTERFACE HTTPReaver : _PINE_EXTENDS public Reaver {
    public:
        virtual Pinecone::Atom* queryBrowserConfig() const = 0;

        virtual BrowserSim* getBrowserSim() const = 0;

    protected:
        virtual std::string getHTTPFile( const std::string& szHref ) const = 0;
    };

    _PINE_INTERFACE RangedReaver : _PINE_EXTENDS public HTTPReaver {
    public:
        virtual Heists::Metier getMetier() const _PINE_NOEXCEPT = 0;

        virtual size_t getLootFrom() const = 0;

        virtual size_t getLootTo() const = 0;

    protected:
        virtual void beforeNextHttpLoot() const { }

        virtual void afterLastHttpLoot() const { }

    public:
        virtual void toRavage() const = 0;

        virtual void toStalk() const = 0;

        virtual void toEmbezzle() const = 0;
    };


    _PINE_ABSTRACT_CLASS _PINE_API_OUT MegaDOMIndexReaver: _PINE_IMPLEMENTS public RangedReaver {
    public:
        struct FailureConf {
            Pinecone::Atom*           lpjFailureConf          = nullptr;
            size_t                    nFailedFileSize        = 2000   ;
            size_t                    nFileRetrieveTime      = 3      ;
        };

    protected:
        std::string               mszIndexPath           ;
        std::string               mszSpoilPath           ;
        std::string               mszHeistHref           ;
        FailureConf               mFailureConf           ;

    protected:
        Pinecone::Atom*           mpConfig          ;
        std::string               mszFragNS         ;
        size_t                    mnFragBase     = 10000      ;  // Unit: W
        size_t                    mnFragRange    = 1000000    ;  // Unit: 1
        size_t                    mnLootFrom        ;
        size_t                    mnLootTo          ;

    protected:
        BrowserSim*               mpBrowserSim   = nullptr   ;

    protected:
        IndexSniffer*             mpIndexSniffer = nullptr   ;
        Heist*                    mpParentHeistum            ;

    protected:
        std::string       mszKingpinHost;
        Pinecone::int16   mnKingpinPort;
        bool              mbEnableGuerrilla = false;
        std::string       mszEdgeClientID;

    protected:
        mutable EdgeHeistClient*  mpEdgeClient = nullptr;

    public:
        explicit MegaDOMIndexReaver( Heist* lpDispatcher ) ;

        MegaDOMIndexReaver( Heist* lpDispatcher, size_t nLootFrom  , size_t nLootTo ) ;

        Reaver* apply( IndexSniffer* lpDesignatedSniffer ) {
            this->mpIndexSniffer = lpDesignatedSniffer;
            return this;
        }

        virtual ~MegaDOMIndexReaver();

    protected:
        virtual void compressSoloArchive( std::string& szHtml, const Pinecone::HTMLDocument* lpDocument ) const  { }

        virtual void storeHrefCache( const std::string& szStoragePath, const std::string& szHref ) const ;

        virtual void lootById( size_t id ) const ;

    public:
        virtual std::string getHTTPFileFromHref( const std::string& szHref ) const ;

        void validateSpoil( const std::string& sz ) const _PINE_OVERRIDE ;

        const Pinecone::Atom* getCrewConfig() const _PINE_NOEXCEPT {
            return this->mpConfig;
        }

        size_t getThresholdFailedFileSize() const _PINE_NOEXCEPT _PINE_OVERRIDE {
            return this->mFailureConf.nFailedFileSize;
        }

    public:
        Heist* parentHeistum() const _PINE_OVERRIDE {
            return this->mpParentHeistum;
        }

        Pinecone::Atom* queryBrowserConfig() const _PINE_OVERRIDE;

        BrowserSim* getBrowserSim() const _PINE_OVERRIDE {
            return this->mpBrowserSim;
        }

        virtual std::string queryHrefById ( size_t id ) const = 0;

        virtual std::string queryFragNamespace ( size_t id ) const {
            id = ( id == 0 ? 1 : id );
            size_t nBase = this->mnFragRange / this->mnFragBase;
            size_t nLow  = id / this->mnFragRange;
            size_t nMod  = id % this->mnFragRange;

            size_t nAbove = nLow;
            if( nMod != 0 || id % 10 == 0 ) {
                ++nAbove;
            }

            nAbove *= nBase;
            nLow   *= nBase;

            std::string szNS = Pinecone::StdString::toString(nLow) + "W";
            szNS.append( "_" ).append( Pinecone::StdString::toString(nAbove) ).append( "W" );

            return szNS;
        }

        virtual std::string querySpoilStorageDir( size_t id ) const {
            return this->mszSpoilPath + this->queryFragNamespace( id ) + "\\";
        }

        virtual std::string querySpoilStoragePath( size_t id ) const {
            return this->querySpoilStorageDir( id ) + "page_" + Pinecone::StdString::toString( id ) + ".html";
        }

    public:
        std::string prototypeName() const _PINE_OVERRIDE {
            return this->className();
        }

        std::string className() const _PINE_OVERRIDE = 0;

        std::string crewName() const _PINE_OVERRIDE  = 0;

    protected:
        virtual void lootFromRange() const ;

        virtual void revitalizeStalker() const ;

        void beforeNextHttpLoot() const _PINE_OVERRIDE ;

        void afterLastHttpLoot() const _PINE_OVERRIDE ;

        std::string getHTTPFile( const std::string& szHref ) const _PINE_OVERRIDE;

    public:
        Heists::Metier getMetier() const _PINE_NOEXCEPT _PINE_OVERRIDE ;

        size_t getLootFrom() const _PINE_OVERRIDE {
            return this->mnLootFrom;
        }

        size_t getLootTo() const _PINE_OVERRIDE {
            return this->mnLootTo;
        }

    protected:
        virtual void beforeSendEdgeRequest() const;

    public:
        bool isEdgeKingpin() const _PINE_NOEXCEPT;

        virtual void sendEdgeSpoil( Pinecone::Atom* lpHead, const std::string& szSpoil ) const;

        virtual void queryEdgeRequest( Pinecone::Atom* lpHead, Pinecone::Atom* lpResponse ) const;

        const std::string& getKingEdgeHost() const _PINE_NOEXCEPT {
            return this->mszKingpinHost;
        }

        const std::string& getEdgeClientID() const _PINE_NOEXCEPT {
            return this->mszEdgeClientID;
        }

        int16_t getKingEdgePort() const _PINE_NOEXCEPT {
            return this->mnKingpinPort;
        }

        virtual void afterEdgeRequestAccept( Pinecone::Atom* lpHead, EdgeHeistProtocol* lpProtocol ) ;

    public:
        void toRavage() const _PINE_OVERRIDE;

        void toStalk() const _PINE_OVERRIDE;

        void toEmbezzle() const _PINE_OVERRIDE;

    public:
        void isTimeToFeast() const _PINE_OVERRIDE ;

    };

}

#endif //NONARON_REAVER_H
