#ifndef SAURON_WIKIHEISTREAVER_H
#define SAURON_WIKIHEISTREAVER_H
#include <Pinecone/Framework/defined.inc.h>
#include <Ulfhedinn/config.inc.h>
#include <Pinecone/Framework/Element/Atom/Atom.h>
#include <Pinecone/Framework/Unit/Dictron/Dictron.h>
#include <Pinecone/Framework/Unit/Dictron/Listron.h>
#include <Pinecone/Framework/Util/JSON/JSON.h>
#include <Pinecone/Framework/Util/File/FileUtils.h>
#include <Pinecone/Framework/Unit/String/StdString.h>
#include <Pinecone/Framework/Util/Tracer/Tracer.h>
#include <Sauron/System/defined.inc.h>
#include "Sauron/Heistron/Reaver.h"
#include "Sauron/Heistron/JsonIndexReaver.h"

namespace Sauron {
    class Heist;

    _PINE_ABSTRACT_CLASS WikiReaver : _PINE_IMPLEMENTS public JsonIndexReaver {
    public:
        mutable Pinecone::Atom    thisom;
        mutable std::ofstream     mofSeg;

    public:
        explicit WikiReaver( Heist* lpDispatcher ) : JsonIndexReaver( lpDispatcher ) {};

        WikiReaver( Heist* lpDispatcher, size_t nLootFrom  , size_t nLootTo ) :
        JsonIndexReaver( lpDispatcher, nLootFrom, nLootTo ) {
        }

    protected:
        void embezzle_index() const;

        void embezzleFromRange() const ;

    protected:
        void embezzle_wikipedia_solo_page( const std::string& szPageCache, Pinecone::Atom& joPage ) const;

        void embezzle_wikipedia_solo_page_to_sql( const std::string& szPageCache ) const;

    protected:
        void embezzle_wiktionary_from_range() const;

        void embezzle_wiktionary_solo_page( const std::string& szPageCache, Pinecone::Atom& joPage ) const;

        void embezzle_wiktionary_solo_page_to_sql( const std::string& szPageCache ) const;

        void embezzle_wiktionary_solo_page_by_id ( size_t id ) const;

    public:
        void validateSpoil( const std::string& sz ) const _PINE_OVERRIDE ;

        void lootById( size_t id ) const;

        void toEmbezzle() const _PINE_OVERRIDE;

    public:
        std::string prototypeName() const _PINE_OVERRIDE {
            return this->className();
        }

        std::string className() const _PINE_OVERRIDE = 0;

        std::string crewName() const _PINE_OVERRIDE  = 0;

    };

    class WikipediaReaver: _PINE_EXTENDS public WikiReaver {
    public:
        explicit WikipediaReaver( Heist* lpDispatcher ) : WikiReaver( lpDispatcher ) {};

        WikipediaReaver(
                Heist* lpDispatcher, size_t nLootFrom  , size_t nLootTo
        ) : WikiReaver( lpDispatcher, nLootFrom, nLootTo ) {
        }

    public:
        std::string className() const _PINE_OVERRIDE {
            return "WikipediaReaver";
        }

        std::string crewName() const _PINE_OVERRIDE  {
            return "Wikipedia";
        }
    };
}

#endif //SAURON_WIKIHEISTREAVER_H
