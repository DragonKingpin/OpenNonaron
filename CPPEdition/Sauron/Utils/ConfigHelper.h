#ifndef NONARON_CONFIGHELPER_H
#define NONARON_CONFIGHELPER_H
#include <Pinecone/Framework/Element/Atom/Atom.h>
#include <Pinecone/Framework/Unit/Dictron/Dictron.h>

namespace Sauron {
    class _PINE_API_OUT ConfigHelper {
    public:
        inline static void evalString( std::string& sz, const std::string& szKey, const Pinecone::Atom& jo ) {
            Pinecone::Atom* that = jo.getDictron()->getValueByKey( szKey );
            if( that ) {
                sz = that->toString();
            }
        }

        inline static void evalString( std::string& sz, const std::string& szKey, const Pinecone::Atom& jo, const std::string& szDefault ) {
            Pinecone::Atom* that = jo.getDictron()->getValueByKey( szKey );
            if( that ) {
                sz = that->toString();
            }
            else {
                sz = szDefault;
            }
        }

        inline static void evalPath  ( std::string& sz, const std::string& szDirPath, const std::string& szKey, const Pinecone::Atom& jo ) {
            Pinecone::Atom* that = jo.getDictron()->getValueByKey( szKey );
            if( that ) {
                sz = szDirPath;
                sz += that->toString();
            }
        }

        inline static void evalSizeT( std::size_t& n, const std::string& szKey, const Pinecone::Atom& jo ) {
            Pinecone::Atom* that = jo.getDictron()->getValueByKey( szKey );
            if( that ) {
                n = that->getRawInt64Zone();
            }
        }

        inline static void evalInt64( std::int64_t& n, const std::string& szKey, const Pinecone::Atom& jo ) {
            Pinecone::Atom* that = jo.getDictron()->getValueByKey( szKey );
            if( that ) {
                n = that->getRawInt64Zone();
            }
        }

        inline static void evalBool( bool& b, const std::string& szKey, const Pinecone::Atom& jo ) {
            Pinecone::Atom* that = jo.getDictron()->getValueByKey( szKey );
            if( that ) {
                b = (bool ) that->getRawInt64Zone();
            }
        }

    public:
        static void overrideConfig( Pinecone::Atom& joOverridden, const Pinecone::Atom& joParent, const Pinecone::Atom& joThat, bool bRecursive ) ;

        inline static void overrideConfig( Pinecone::Atom& joOverridden, const Pinecone::Atom& joParent, const Pinecone::Atom& joThat ) {
            ConfigHelper::overrideConfig( joOverridden, joParent, joThat, true );
        }
    };
}

#endif //NONARON_CONFIGHELPER_H
