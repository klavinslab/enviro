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

#endif