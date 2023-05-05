#include <Sauron/Heistron/Heistron.h>
#include "WikiHeist.h"
#include "WikiReaver.h"

namespace Sauron {

    void WikipediaHeist::embezzle_wiktionary() {
        this->mnMaximumAllocate = this->mnSumTasks / this->mnEachTasksPerThread + 1;
        char* buf = new char[ 512 ];
        sprintf(
                buf, "{ From:%lld, To:%lld, BatchDesired:%lld, Tasks:%lld, Each:%lld, IdxSum:%lld }",
                (int64)this->mnTakeFrom, (int64)this->mnTakeTo, (int64)this->mnMaximumAllocate,
                (int64)this->mnSumTasks, (int64)this->mnEachTasksPerThread, (int64)this->mjIndex.size()
        );
        this->sysInfo( { buf } );
        delete[] buf;

        //std::ios::sync_with_stdio( false );
        this->polling();

        this->trace( { "[SuccessfullyHeist::Embezzle]" } );
    }

    void WikipediaHeist::embezzle_wikipedia() {
        WikipediaReaver reaver( this );
        reaver.isTimeToFeast();
    }

    void WikipediaHeist::vitalizeThreadByRange( size_t i, size_t nFrom, size_t nTo ) {
        this->trace( { "[ThreadVitalized]: ", i, nFrom, nTo, Self::evalDatetime() } );
        //std::this_thread::sleep_for( std::chrono::milliseconds(1000) );

        WikipediaReaver reaver( this, nFrom, nTo );
        reaver.isTimeToFeast();

        this->afterThreadTerminated();
    }

    void WikipediaHeist::vitalizeStalker() {
        WikipediaReaver reaver( this );
        reaver.isTimeToFeast();
    }

    void WikipediaHeist::toEmbezzle() {
        if( this->mszInstanceExtend == "Wiktionary" ) {
            this->embezzle_wiktionary();
//            WikipediaReaver reaver( this );
//            reaver.isTimeToFeast();
        }
        else {
            this->embezzle_wikipedia();
        }
    }

    HEISTUM_REGISTER( WikipediaHeist );
}