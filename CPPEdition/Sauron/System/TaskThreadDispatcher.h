#ifndef SAURON_TASKTHREADDISPATCHER_H
#define SAURON_TASKTHREADDISPATCHER_H
#include <Pinecone/Framework/defined.inc.h>
#include <Ulfhedinn/config.inc.h>
#include <thread>
#include <mutex>
#include <Pinecone/Framework/Unit/Map/LinkedTreeMap.h>
#include <Pinecone/Framework/Util/Tracer/Tracer.h>
#include <Pinecone/Framework/System/Exception/Exception.h>
namespace Sauron {
    class _PINE_API_OUT TaskTerminatedException: public Pinecone::Exception {
    public:
        TaskTerminatedException() : Exception( "TaskTerminatedException: No information given." ) {
        }

        explicit TaskTerminatedException( const char *message ) : Exception( message ) {
        }

        explicit TaskTerminatedException( const std::string &message) : Exception( message ) {
        }

        TaskTerminatedException( const char *pszMessage , const char * pszLocated, Pinecone::int32 nLineAt ) : Exception( pszMessage, pszLocated, nLineAt) {
        }

        TaskTerminatedException( const std::string &msg, const char * pszLocated, Pinecone::int32 nLineAt ) : Exception( msg, pszLocated, nLineAt) {
        }

        TaskTerminatedException( const std::string &msg, Throwable *cause ) : Exception( msg, cause ) {
        }

    public:
        std::string toString() const _PINE_OVERRIDE {
            return "[object TaskTerminatedException]";
        }

        std::string prototypeName() const _PINE_OVERRIDE {
            return "TaskTerminatedException";
        }
    };

    _PINE_ABSTRACT_CLASS _PINE_API_OUT TaskThreadDispatcher {
    public:
        struct Task {
            size_t          mnEpochId;
            size_t          mnTaskFrom;
            size_t          mnTaskTo;
            std::thread*    mpThread;
        };

    public:
        typedef Pinecone::LinkedTreeMap<std::thread::id, Task* >  pool_type;

    protected:
        mutable volatile size_t mnCurrentTheadSum = 0;
        mutable volatile size_t mnMutualSum       = 0;
        mutable volatile size_t mnSpeed           = 0;

    protected:
        size_t mnMaximumThread            = 0;
        size_t mnTakeFrom                 = 0;
        size_t mnTakeTo                   = 400000; //16906325
        size_t mnEachTasksPerThread       = 50000;

    protected:
        size_t mnCurrEpoch                = 0;
        size_t mnMaximumAllocate          = 0;

    protected:
        size_t      mnStepTask ;
        size_t      mnSumTasks ;

    protected:
        mutable pool_type   mThreadPool;
        mutable std::mutex  mMuxDispatch;
        mutable std::mutex  mMuxPool;

    protected:
        size_t evalNextRange ( size_t nLastTo ) const _PINE_NOEXCEPT {
            if( nLastTo + this->mnEachTasksPerThread > this->mnSumTasks ) {
                return this->mnSumTasks;
            }
            return nLastTo + this->mnEachTasksPerThread;
        }

        void afterThreadTerminated() const ;

        virtual void vitalizeThreadByRange( size_t i, size_t nFrom, size_t nTo ) = 0;

        virtual void afterThreadAllocated() {}

        virtual void afterDispatch() {}

        virtual void afterPolling() {}

    public:
        TaskThreadDispatcher() {
            this->mnStepTask = this->mnTakeFrom;
            this->mnSumTasks = this->mnTakeTo - this->mnTakeFrom;
        }

    public:
        virtual void allocate() ;

        virtual void dispatch();

        virtual void polling();

    };
}


#endif //SAURON_TASKTHREADDISPATCHER_H
