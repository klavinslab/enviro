#ifndef __WANDERER_AGENT__H
#define __WANDERER_AGENT__H 

#include <string>
#include <math.h>
#include "enviro.h"

namespace {

    using namespace enviro;

    class MovingForward : public State, public AgentInterface {
        public:
        MovingForward() : State("Moving Forward") {}
        void entry(const Event& e) {}
        void during() { 
            agent->servo(2, 0); 
        }
        void exit(const Event& e) {}
    };

    class Rotating : public State, public AgentInterface {
        public: 
        Rotating() : State("Rotating") {}
        void entry(const Event& e) { 
            rate = rand() % 2 == 0 ? -0.25 : 0.25;  
        }
        void during() { 
            agent->servo(0,rate);  
        }
        void exit(const Event& e) {}
        double rate;
    };

    class WandererController : public StateMachine, public AgentInterface {

        public:
        WandererController() : StateMachine() {
            set_initial(moving_forward);
            add_transition("random_tick", moving_forward, rotating);
            add_transition("random_tick", rotating, moving_forward);
        }

        void update() {
            if ( rand() % 100 <= 5 ) {
                emit(Event("random_tick"));
            }        
            StateMachine::update();
        }

        MovingForward moving_forward;
        Rotating rotating;

    };

    class Wanderer : public Agent {

        public:
        Wanderer(json spec, World& world) : Agent(spec, world) {
            add_process(wc);
        }

        private:
        WandererController wc;

    };

    DECLARE_INTERFACE(Wanderer);

}

#endif