#ifndef NONARON_HEISTS_H
#define NONARON_HEISTS_H
#include <Pinecone/Framework/defined.inc.h>
#include <Ulfhedinn/config.inc.h>
#include <string>

namespace Sauron {
    class Heists {
    public:
        enum HeistType {
            HT_TEMPLATED = 0x01,
            HT_SOPHISTICATE,
            HT_PERIOD
        };

        enum Metier {
            M_REAVER = 0x01,
            M_STALKER,
            M_EMBEZZLER
        };

    public:
        inline static HeistType queryHeistType ( const std::string& szHeistType ){
            if( szHeistType == "Templated" ) {
                return HeistType::HT_TEMPLATED;
            }
            else if( szHeistType == "Sophisticate" ) {
                return HeistType::HT_SOPHISTICATE;
            }
            else if( szHeistType == "Period" ) {
                return HeistType::HT_PERIOD;
            }

            return HeistType::HT_TEMPLATED;
        }

        inline static Metier queryMetier ( const std::string& szMetier ){
            if( szMetier == "Reaver" ) {
                return Metier::M_REAVER;
            }
            else if( szMetier == "Stalker" ) {
                return Metier::M_STALKER;
            }
            else if( szMetier == "Embezzler" ) {
                return Metier::M_EMBEZZLER;
            }

            return Metier::M_REAVER;
        }
    };

}

#endif //NONARON_HEISTS_H
