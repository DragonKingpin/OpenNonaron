#ifndef NONARON_DEFINED_INC_H
#define NONARON_DEFINED_INC_H
#include <Pinecone/Framework/System/stdc++.h>
#include <Pinecone/Framework/System/stdtype.def.h>
#include "config.inc.h"

namespace Sauron {
    typedef Pinecone::ureg_t             ureg_t      ; // 32bits should be 'uint32', 64bits should be 'uint64'
    typedef Pinecone::reg_t              reg_t       ; // 32bits should be 'int32' , 64bits should be 'int64'

    typedef Pinecone::byte                   byte    ;
    typedef Pinecone::int8                   int8    ;
    typedef Pinecone::uint8                  uint8   ;
    typedef Pinecone::int16                  int16   ;
    typedef Pinecone::uint16                 uint16  ;
    typedef Pinecone::int32                  int32   ;
    typedef Pinecone::uint32                 uint32  ;
    typedef Pinecone::uint64                 uint64  ;
    typedef Pinecone::int64                  int64   ;
    typedef Pinecone::float32                float32 ;
    typedef Pinecone::float64                float64 ;
}


#endif //NONARON_DEFINED_INC_H
