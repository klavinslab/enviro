#ifndef __ROBOT_AGENT__H
#define __ROBOT_AGENT__H 

#include "enviro.h"
#include "math.h"

using namespace enviro;

class RobotController : public Process, public AgentInterface {

    public:
    RobotController() : Process(), AgentInterface() {}

    void init() {
        desired_heading = 0;
        counter = 0;
    }
    void start() {}
    void update() {
        if ( counter++ > 50 ) {
            desired_heading += M_PI / 2;
            counter = 0;
        }
        track_velocity(20,30*sin(desired_heading-angle()) - 100*angular_velocity());
        label(std::to_string(((int) (2*desired_heading/M_PI))/2.0) + " pi",15,-15);
    }
    void stop() {}

    int counter;
    double desired_heading;

};

class Robot : public Agent {
    public:
    Robot(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    RobotController c;
};

DECLARE_INTERFACE(Robot)

#endif