#ifndef __WANDERER_AGENT__H
#define __WANDERER_AGENT__H 

#include <string>
#include <math.h>
#include "enviro.h"

namespace {

    using namespace enviro;

    class MovingForward : public State, public AgentInterface {
        void entry(const Event& e) {}
        void during() { agent->servo(3, 0); }
        void exit(const Event& e) {}
    };

    class Rotating : public State, public AgentInterface {
        void entry(const Event& e) { rate = rand() % 2 == 0 ? -0.15 : 0.15; }
        void during() { agent->servo(0,rate); }
        void exit(const Event& e) {}
        double rate;
    };

    class WandererController : public StateMachine, public AgentInterface {

        public:
        WandererController() : StateMachine() {
            set_initial(moving_forward);
            tick_name = std::to_string(rand()%10000); // use a randomly generated event name in case there are 
                                                      // multiple instances of this class
            add_transition(tick_name, moving_forward, rotating);
            add_transition(tick_name, rotating, moving_forward);
        }

        void update() {
            if ( rand() % 100 <= 5 ) {
                emit(Event(tick_name));
            }        
            StateMachine::update();
        }

        MovingForward moving_forward;
        Rotating rotating;
        std::string tick_name;

    };

    class Wanderer : public Agent {

        public:
        Wanderer(json spec, World& world) : Agent(spec, world) {
            add_process(wc);
        }

        WandererController wc;

    };

    DECLARE_INTERFACE(Wanderer);

}

#endif