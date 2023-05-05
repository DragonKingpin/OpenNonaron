#include "SysDaemon.h"
#include "System.h"
#include <Pinecone/Framework/Element/Atom/Atom.h>
#include <Ulfhedinn/Util/Runable/WinProcesses.h>
#include <Pinecone/Framework/Unit/Dictron/Maptron.h>

namespace Sauron {
    SysDaemon::SysDaemon( System *lpSystem ) :mpSystem( lpSystem ) {
        this->mpDaemonsConfig  = &(*this->mpSystem->getSystemConfig())[ "Daemons" ];
        this->mpWatchingConfig = &(*this->mpDaemonsConfig)[ "WatchList" ];
    }

    void SysDaemon::keepHostAlive() const {
        while ( true ) {
            Pinecone::LinkedTreeMap<std::string, bool > watchingMap;
            PineAtom& jaWatchingList = *this->mpWatchingConfig;
            for ( int i = 0; i < jaWatchingList.size(); ++i ) {
                watchingMap[ Pinecone::StdString::toLowerCase( jaWatchingList[i].toString() ) ] = false;
            }

            Pinecone::ArrayList<PROCESSENTRY32 > procList;
            Ulfhedinn::WinProcesses::fetchProcessListRaw( procList );
            for ( int i = 0; i < procList.size(); ++i ) {
                std::string sz = Pinecone::StdString::toLowerCase( procList[i].szExeFile );
                if( watchingMap.hasOwnProperty( sz ) ) {
                    watchingMap[ sz ] = true;
                }
            }

            for ( auto& sz : watchingMap ) {
                echo sz.Key;
                if( !sz.Value ) {
                    echo "Revitalizing...", endp;
                    system( ( "START " + sz.Key ).c_str() );
                }
                else {
                    echo "Alive", endp;
                }
            }

            std::this_thread::sleep_for( std::chrono::milliseconds(1000) );
        }
    }
}