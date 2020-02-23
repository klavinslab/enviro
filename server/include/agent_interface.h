#ifndef __ENVIRO_CONTROLLER__H
#define __ENVIRO_CONTROLLER__H 

#include "enviro.h"

using namespace elma;

namespace enviro {

    class AgentInterface {

        public: 
        inline void use_agent(Agent &a) { agent = &a; }

        virtual ~AgentInterface() {}

        protected:
        Agent * agent;

    };

}

#define LINK_STATES \
    for ( Transition& t : _transitions ) { \
        AgentInterface * ai = dynamic_cast<AgentInterface *>(&t.from()); \
        ai->use_agent(*agent); \
    } 

#endif