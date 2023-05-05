#ifndef NONARON_MEESEEKSBOX_H
#define NONARON_MEESEEKSBOX_H
#include <Pinecone/Framework/defined.inc.h>
#include <Sauron/System/defined.inc.h>
#include <Pinecone/Framework/Unit/String/StdString.h>
#include <Pinecone/Framework/Unit/Map/LinkedTreeMap.h>
#include <string>
#include "Meeseek.h"

namespace Pinecone {
    class Atom;
}

namespace Sauron {
    class System;

    class _PINE_API_OUT MeeseeksBox {
    public:
        enum MissionMode {
            HM_DESIGNATE = 0x01,
            HM_SCHEDULE
        };

    public:
        typedef Pinecone::Atom                                                PineAtom          ;
        typedef Pinecone::LinkedTreeMap<std::string, Meeseek* >               pool_type         ;

    private:
        MissionMode                     mMissionMode         = MissionMode::HM_DESIGNATE;
        std::string                     mszPrimeDirective              ;
        PineAtom*                       mpMissionConfig      = nullptr ;
        PineAtom*                       mpMeeseeksList       = nullptr ;

    private:
        Meeseek*                        mpPrimeMeeseek       = nullptr ;
        pool_type                       mMeeseekPool                   ;

    private:
        System*                         mpSystem                       ;

    public:
        explicit MeeseeksBox( System* lpSystem ) ;

        ~MeeseeksBox();

    private:
        void init() ;

        void loadConfig();

        void meltdownAll();

    public:
        System* getParentSystem() _PINE_SIMPLE_GETTER {
            return this->mpSystem;
        }

    public:
        const std::string& getPrimeDirective() const {
            return this->mszPrimeDirective;
        }

        PineAtom* getMissionConfig() _PINE_SIMPLE_GETTER {
            return this->mpMissionConfig;
        }

        PineAtom* getMeeseeksList() _PINE_SIMPLE_GETTER {
            return this->mpMeeseeksList;
        }

        Meeseek* getPrimeMeeseek() _PINE_SIMPLE_GETTER {
            return this->mpPrimeMeeseek;
        }

    public:
        Meeseek* spawn( const std::string& szName ) ;

        void free ( Meeseek*& that ) ;

        void free ( const std::string& szName ) ;

    private:
        void vitalizeDetachedMeeseek();

        void vitalizePrimeMeeseek();

        void dispatch();

        void waitForAllSought();

    public:
        void toExert();

    };
}

#endif //NONARON_MEESEEKSBOX_H
