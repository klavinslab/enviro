#ifndef ___DEF_NAME__AGENT__H
#define ___DEF_NAME__AGENT__H 

#include "enviro.h"

using namespace enviro;

class _CLASS_NAME_Controller : public Process, public AgentInterface {

    public:
    _CLASS_NAME_Controller() : Process(), AgentInterface() {}

    void init() {}
    void start() {}
    void update() {}
    void stop() {}

};

class _CLASS_NAME_ : public Agent {
    public:
    _CLASS_NAME_(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    _CLASS_NAME_Controller c;
};

DECLARE_INTERFACE(_CLASS_NAME_)

#endif