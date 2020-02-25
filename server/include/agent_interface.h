#ifndef __ENVIRO_CONTROLLER__H
#define __ENVIRO_CONTROLLER__H 

#include "enviro.h"

using namespace elma;

namespace enviro {

    class AgentInterface {

        public: 
        inline void use_agent(Agent &a) { agent = &a; }

        // State
        cpVect position();
        cpVect velocity();
        cpFloat angle();
        cpFloat angular_velocity();
        int id();

        // Actuators
        void apply_force(cpFloat thrust, cpFloat torque);
        void track_velocity(cpFloat linear_velocity, cpFloat angular_velocity, 
                            cpFloat kL=10, cpFloat kR=200);
        void damp_movement();      

        // Sensors
        double sensor_value(int index);
        std::vector<double> sensor_values();

        virtual ~AgentInterface() {} // needed to make dynamic_cast work

        protected:
        Agent * agent;

    };

}

#endif