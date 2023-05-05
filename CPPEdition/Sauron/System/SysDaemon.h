#ifndef NONARON_SYSDAEMON_H
#define NONARON_SYSDAEMON_H
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
    class System;

    class _PINE_API_OUT SysDaemon {
    public:
        typedef Pinecone::Atom                                                PineAtom          ;

    protected:
        System*                         mpSystem                       ;
        PineAtom*                       mpDaemonsConfig      = nullptr ;
        PineAtom*                       mpWatchingConfig     = nullptr ;

    public:
        explicit SysDaemon( System* lpSystem ) ;

    public:
        void keepHostAlive() const;

    };
}

#endif //NONARON_SYSDAEMON_H
