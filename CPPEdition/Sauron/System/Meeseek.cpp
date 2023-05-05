#include <Pinecone/Framework/Element/Atom/Atom.h>
#include "Meeseek.h"
#include "MeeseeksBox.h"
#include <Sauron/System/System.h>

namespace Sauron {
    Meeseek::Meeseek( const std::string &szName, MeeseeksBox *lpBox )  :
    mpMeeseeksBox( lpBox ), mSeekType( Meeseek::SeekType::ST_PRIME ),
    mpMeeseekConf( nullptr ), mbTraceLifecycle( false ) {
        this->mszName = szName;
    }

    void Meeseek::traceInitInfo() const {
        if( this->mbTraceLifecycle ) {
            this->getParentSystem()->traceLifecycleDetails( "[MeeseekSpawned] <I'm Mr.Meeseek[" + this->mszName + "], look at me !>" );
        }
    }

    void Meeseek::loadConfig() {
        PineAtom& jMeeseekConf   = ( *this->mpMeeseeksBox->getMeeseeksList() )[ this->mszName ];
        this->mpMeeseekConf      = &jMeeseekConf;
        this->mbTraceLifecycle   = (bool) jMeeseekConf[ "TraceLifecycle" ].getRawInt64Zone();

        std::string szSeekType   = jMeeseekConf[ "SeekType" ].toString();
        if( szSeekType == "Prime" ) {
            this->mSeekType = Meeseek::SeekType::ST_PRIME;
        }
        else if( szSeekType == "Affiliate" ) {
            this->mSeekType = Meeseek::SeekType::ST_AFFILIATE;
        }
        else if( szSeekType == "Detached" ) {
            this->mSeekType = Meeseek::SeekType::ST_DETACHED;
        }
        else if( szSeekType == "Joint" ) {
            this->mSeekType = Meeseek::SeekType::ST_JOINT;
        }

    }

    System *Meeseek::getParentSystem() _PINE_SIMPLE_GETTER {
        return this->mpMeeseeksBox->getParentSystem();
    }
}