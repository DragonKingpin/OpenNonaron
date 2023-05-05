#ifndef NONARON_WIKIHEIST_H
#define NONARON_WIKIHEIST_H
#include "Sauron/Heistron/Heist.h"

namespace Sauron {
    _PINE_ABSTRACT_CLASS WikiHeist: _PINE_EXTENDS public Heist {
    public:
        explicit WikiHeist( Heistron* lpHeistron ): Heist( lpHeistron ) {

        }

    public:
        std::string prototypeName() const _PINE_OVERRIDE {
            return this->className();
        }

        std::string className() const _PINE_OVERRIDE {
            return "WikiHeist";
        }

        std::string heistName() const _PINE_OVERRIDE = 0 ;
    };


    class WikipediaHeist: _PINE_EXTENDS public WikiHeist {
    public:
        explicit WikipediaHeist( Heistron* lpHeistron ): WikiHeist( lpHeistron ) {
            this->WikipediaHeist::init();
        }

    protected:
        void embezzle_wiktionary();

        void embezzle_wikipedia();

    public:
        void vitalizeThreadByRange( size_t i, size_t nFrom, size_t nTo ) _PINE_OVERRIDE ;

        void vitalizeStalker() _PINE_OVERRIDE ;

        void toEmbezzle() _PINE_OVERRIDE;

    public:
        std::string prototypeName() const _PINE_OVERRIDE {
            return this->className();
        }

        std::string className() const _PINE_OVERRIDE {
            return "WikipediaHeist";
        }

        std::string heistName() const _PINE_OVERRIDE {
            return "Wikipedia";
        }
    };

}


#endif //NONARON_WIKIHEIST_H
