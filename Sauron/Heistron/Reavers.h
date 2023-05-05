#ifndef NONARON_REAVERS_H
#define NONARON_REAVERS_H
#include <string>
#include <Pinecone/Framework/defined.inc.h>
#include <Ulfhedinn/config.inc.h>
#include <Pinecone/Framework/System/Exception/Exception.h>

namespace Sauron {

    class LootRecoveredException : public Pinecone::Exception {
    public:
        LootRecoveredException() : Exception( "LootRecoveredException: No information given." ) {
        }

        explicit LootRecoveredException( const char *message ) : Exception( message ) {
        }

        explicit LootRecoveredException( const std::string &message) : Exception( message ) {
        }

        LootRecoveredException( const char *pszMessage , const char * pszLocated, Pinecone::int32 nLineAt ) : Exception( pszMessage, pszLocated, nLineAt) {
        }

        LootRecoveredException( const std::string &msg, const char * pszLocated, Pinecone::int32 nLineAt ) : Exception( msg, pszLocated, nLineAt) {
        }

        LootRecoveredException( const std::string &msg, Throwable *cause ) : Exception( msg, cause ) {
        }

    public:
        std::string toString() const _PINE_OVERRIDE {
            return "[object LootRecoveredException]";
        }

        std::string prototypeName() const _PINE_OVERRIDE {
            return "LootRecoveredException";
        }

    };

    class LootAbortException : public Pinecone::Exception {
    public:
        LootAbortException() : Exception( "LootAbortException: No information given." ) {
        }

        explicit LootAbortException( const char *message ) : Exception( message ) {
        }

        explicit LootAbortException( const std::string &message) : Exception( message ) {
        }

        LootAbortException( const char *pszMessage , const char * pszLocated, Pinecone::int32 nLineAt ) : Exception( pszMessage, pszLocated, nLineAt) {
        }

        LootAbortException( const std::string &msg, const char * pszLocated, Pinecone::int32 nLineAt ) : Exception( msg, pszLocated, nLineAt) {
        }

        LootAbortException( const std::string &msg, Throwable *cause ) : Exception( msg, cause ) {
        }

    public:
        std::string toString() const _PINE_OVERRIDE {
            return "[object LootRecoveredException]";
        }

        std::string prototypeName() const _PINE_OVERRIDE {
            return "LootRecoveredException";
        }

    };

}
#endif //NONARON_REAVERS_H
