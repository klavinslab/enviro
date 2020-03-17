#ifndef __VIRUS_AGENT__H
#define __VIRUS_AGENT__H 

#include "enviro.h"
#include "styles.h"
#include <math.h>

using namespace enviro;

class VirusController : public Process, public AgentInterface {

    public:
    VirusController() : Process(), AgentInterface(), hit(false) {}

    void init() {
        notice_collisions_with("Bullet", [&](Event &e) {
            Agent& bullet = find_agent(e.value()["id"]);
            vx = bullet.velocity().x;
            vy = bullet.velocity().y;
            ignore_collisions_with("Bullet");
            hit = true;
        });          
    }

    void start() {}

    void pop() {
        for ( double theta=0; theta < 2 * M_PI; theta += M_PI / 4) {
            Agent& frag = add_agent("VirusFragment", x(), y(), theta, VIRUS_FRAGMENT_STYLE);
            frag.omni_apply_force(
                50*cos(theta+M_PI/8) + vx, 
                50*sin(theta+M_PI/8) + vy
            );
        }  
        remove_agent(id());
    }    

    void update() {
        double d = sqrt(x()*x() + y()*y()),
               vx = -x() / ( 1 + d ),
               vy = -y() / ( 1 + d );        
        omni_apply_force(
            (rand() % fmax) - fmax/2 + 2*vx, 
            (rand() % fmax) - fmax/2 + 2*vy
        );
        if ( hit ) {
            pop();
        } 
    }

    void stop() {}

    const int fmax = 100.0;
    bool hit;
    double vx, vy;         

};

class Virus : public Agent {
    public:
    Virus(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    VirusController c;
};

DECLARE_INTERFACE(Virus)

#endif
