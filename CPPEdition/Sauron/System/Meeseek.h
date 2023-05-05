#ifndef NONARON_MEESEEK_H
#define NONARON_MEESEEK_H
#include <Pinecone/Framework/defined.inc.h>
#include <Sauron/System/defined.inc.h>
#include <string>
#include <thread>
#include <Sauron/System/Prototype/Mission.h>

namespace Pinecone {
    class Atom;
}

namespace Sauron {
    class MeeseeksBox;

    class System;

    _PINE_ABSTRACT_CLASS _PINE_API_OUT Meeseek: _PINE_IMPLEMENTS public Mission {
    public:
        typedef Pinecone::Atom                       PineAtom          ;

    public:
        enum SeekType {
            ST_PRIME                     = 0x01 ,  // Run in main-process-main-thread.
            ST_AFFILIATE                        ,  // Run in main-process-sub-thread.
            ST_DETACHED                         ,  // Run in sub-process-main-thread.

            // Joint-mission is special detached-mission which has distributed sub-mission with independent dispatch-system.
            ST_JOINT                            ,
        };

    protected:
        std::string                     mszName          ;
        SeekType                        mSeekType        ;
        MeeseeksBox*                    mpMeeseeksBox    ;
        PineAtom*                       mpMeeseekConf    ;
        bool                            mbTraceLifecycle ;

    public:
        Meeseek( const std::string& szName, MeeseeksBox* lpBox );

        virtual ~Meeseek() = default;

    protected:
        virtual void loadConfig();

        void traceInitInfo() const ;

    public:
        bool isTraceLifecycle() _PINE_SIMPLE_GETTER {
            return this->mbTraceLifecycle;
        }

        MeeseeksBox* whichBox() _PINE_SIMPLE_GETTER {
            return this->mpMeeseeksBox;
        }

        std::string missionName() const _PINE_OVERRIDE {
            return this->mszName;
        }

        System* getParentSystem() _PINE_SIMPLE_GETTER ;

        Executum* parentExecutum() const _PINE_NOEXCEPT _PINE_OVERRIDE {
            return nullptr;
        }

        PineAtom* getMeeseekConf() const _PINE_NOEXCEPT {
            return this->mpMeeseekConf;
        }

    public:
        virtual void meltdown() = 0;

        virtual void toSeek() = 0;

    };
}

#endif //NONARON_MEESEEK_H
