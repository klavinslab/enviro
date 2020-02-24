#ifndef __ENVIRO_SENSOR__H
#define __ENVIRO_SENSOR__H 

#include "enviro.h"

namespace enviro {

    using namespace elma;

    class Agent;
    class World;

    class Sensor {

        public:
        Sensor(Agent &agent, double x, double y, double angle) 
              : _agent_ptr(&agent), _location({x: x, y: y}), _angle(angle) {
        }

        virtual double value() = 0;

        protected:
        Agent * _agent_ptr; 
        cpVect _location;
        cpFloat _angle;

    };

    class RangeSensor : public Sensor {

        public:
        RangeSensor(Agent &agent, double x, double y, double angle) : Sensor(agent,x,y,angle) {}
        double value();

    };

}

#endif
