#ifndef NONARON_PATHROUTER_H
#define NONARON_PATHROUTER_H
#include <Pinecone/Framework/defined.inc.h>
#include <Pinecone/Framework/Util/Tracer/Tracer.h>
#include <Sauron/System/defined.inc.h>
#include <string>
#include <fstream>

namespace Pinecone {
    class Atom;
}

namespace Sauron {
    class System;

    class _PINE_API_OUT PathRouter {
    public:
        typedef Pinecone::Atom           PineAtom;

    protected:
        System*       mpSystem            ;
        PineAtom*     mpjoPathRouter      ;
        PineAtom*     mpjoReinterpretTab  ;
        PineAtom*     mpReTabDictionary   ;

    public:
        explicit PathRouter( System* lpSystem );

        ~PathRouter();

    protected:
        void reinterpretReTab();

    public:
        PineAtom* getPathRouter() const _PINE_NOEXCEPT {
            return this->mpjoPathRouter;
        }

        PineAtom* getReinterpretTab() const _PINE_NOEXCEPT {
            return this->mpjoReinterpretTab;
        }

        PineAtom* getReTabDictionary() const _PINE_NOEXCEPT {
            return this->mpReTabDictionary;
        }

    };
}


#endif //NONARON_PATHROUTER_H
