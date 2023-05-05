#include "TaskThreadDispatcher.h"

namespace Sauron {
    void TaskThreadDispatcher::afterThreadTerminated() const {
        this->mMuxPool.lock();
        Task* lpTask = this->mThreadPool[ std::this_thread::get_id() ];
        delete lpTask->mpThread;
        delete lpTask;
        this->mThreadPool.erase( std::this_thread::get_id() );
        this->mMuxPool.unlock();

        this->mMuxDispatch.lock();
        --this->mnCurrentTheadSum;
        this->mMuxDispatch.unlock();
    }

    void TaskThreadDispatcher::allocate() {
        this->mMuxDispatch.lock();

        //this->mnMaximumAllocate = this->mnSumTasks / this->mnEachTasksPerThread + 1;
        size_t nAllocations = this->mnMaximumThread - this->mnCurrentTheadSum;
        size_t nDet         = this->mnMaximumAllocate - this->mnCurrEpoch;
        if( nDet < nAllocations ){
            nAllocations = nDet;
        }

        //mutexBootQueueSynchronized.lock();
        for ( size_t i = 0; i < nAllocations; ++i ) {
            ++this->mnCurrentTheadSum;
            size_t nF = this->evalNextRange( (this->mnCurrEpoch - 1) * this->mnEachTasksPerThread ) + this->mnStepTask;
            size_t nT = this->evalNextRange( this->mnCurrEpoch * this->mnEachTasksPerThread ) + this->mnStepTask;

            Task* lpTask = new Task { this->mnCurrEpoch, nF, nT, nullptr };
            std::thread* lpNeoThread = new std::thread( [&]{
                this->mMuxPool.lock();
                lpTask->mpThread = lpNeoThread;
                this->mThreadPool[ std::this_thread::get_id() ] = lpTask;
                this->mMuxPool.unlock();

                this->vitalizeThreadByRange( this->mnCurrEpoch, nF, nT );
            } );
            lpNeoThread->detach();

            ++this->mnCurrEpoch;
            std::this_thread::sleep_for( std::chrono::milliseconds(100) );
        }

        this->afterThreadAllocated();

        this->mMuxDispatch.unlock();
    }

    void TaskThreadDispatcher::dispatch() {
        size_t nMutualSumLast = this->mnMutualSum;
        if( this->mnCurrentTheadSum < this->mnMaximumThread && this->mnCurrEpoch < this->mnMaximumAllocate ) {
            this->allocate();
        }
        else if( this->mnCurrentTheadSum <= 0 ) {
            throw TaskTerminatedException();
        }

        std::this_thread::sleep_for( std::chrono::milliseconds(500) );

        this->mnSpeed = this->mnMutualSum - nMutualSumLast;

        this->afterDispatch();
    }

    void TaskThreadDispatcher::polling() {
        while ( true ) {
            try {
                this->dispatch();
            }
            catch ( TaskTerminatedException& e ) {
                break;
            }
        }

        this->afterPolling();
    }
}