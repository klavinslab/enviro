#ifndef __MESSAGE_AGENT__H
#define __MESSAGE_AGENT__H 

#include "enviro.h"

using namespace enviro;

class MessageController : public Process, public AgentInterface {

    public:
    MessageController() : Process(), AgentInterface() {}

    void init() {}
    void start() {}
    void update() {}
    void stop() {}

};

class Message : public Agent {
    public:
    Message(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    MessageController c;
};

DECLARE_INTERFACE(Message)

#endif