#include "ConfigHelper.h"

namespace Sauron {
    void ConfigHelper::overrideConfig( Pinecone::Atom &joOverridden, const Pinecone::Atom &joParent, const Pinecone::Atom &joThat, bool bRecursive ) {
        joParent.getDictron()->affirmMaptron()->forEach( [&]( int, Pinecone::Maptron::T_KeyValue& kv )->void {
            if( Pinecone::StdString::startsWith( kv.Key, "_", 0 ) ){
                return;
            }

            if( joThat.hasOwnProperty( kv.Key ) ) { // Override only here, and use recursive way.
                Pinecone::Atom & joSubVal = joThat.getDictron()->affirmMaptron()->getKeyValueByKey(kv.Key)->Value;
                if( kv.Value.isDictron() && kv.Value.affirmDictron()->isMap() ) {
                    if( joSubVal.isDictron() && joSubVal.affirmDictron()->isMap() ) {
                        if( bRecursive ) {
                            ConfigHelper::overrideConfig( joOverridden[ kv.Key ], kv.Value, joSubVal, bRecursive );
                        }
                        return ;
                    }
                }

                joOverridden[ kv.Key ] = joSubVal;
            }
            else {
                joOverridden[ kv.Key ] = kv.Value;
            }
        } );

        if( joThat.isDictron() ){
            joThat.getDictron()->affirmMaptron()->forEach( [&]( int, Pinecone::Maptron::T_KeyValue& kv ){
                if( Pinecone::StdString::startsWith( kv.Key, "_", 0 ) ){
                    return;
                }

                if( !joOverridden.hasOwnProperty( kv.Key ) ) {
                    joOverridden[ kv.Key ] = kv.Value;
                }
            } );
        }
        else if( joThat.isInvalid() ){
            return;
        }
        else {
            throw Pinecone::IllegalArgumentException("IllegalArgumentException: [ConfigHelper] Overridden by mismatch datatype.");
        }
    }
}