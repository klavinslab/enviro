#ifndef __CELL_AGENT__H
#define __CELL_AGENT__H 

#include "enviro.h"
#include "styles.h"
#include <math.h>

using namespace enviro;

class CellController : public Process, public AgentInterface {

    public:
    CellController() : Process(), AgentInterface(), infected(false), counter(0) {}

    void init() {
        notice_collisions_with("Virus", [&](Event &e) {
            int virus_id = e.value()["id"];
            Agent &virus = find_agent(virus_id);
            virus_ids.push_back(virus_id);
            attach_to(virus);
            if ( ! infected ) {
                counter = 0;
                infected = true;
            }
        });        
    }

    void start() {}

    void update() {
        double d = sqrt(x()*x() + y()*y()),
               vx = -x() / ( 1 + d ),
               vy = -y() / ( 1 + d );
        omni_apply_force(
            (rand() % fmax) - fmax/2 + 5*vx, 
            (rand() % fmax) - fmax/2 + 5*vy
        );
        if ( infected ) {
            counter++;
            set_style(infected_style());
            show_baby_viruses();
        }
        if ( counter > counter_max ) {
            pop();
        }
    }

    void stop() {}

    std::string interpolate_colors(int r1,int g1,int b1,int r2,int g2,int b2) {
        char buffer[256];
        double f = 1 - ( (double) counter ) / counter_max;
        sprintf(buffer, 
          "rgb(%d,%d,%d)", 
          (int) ( r1*f + r2*(1-f) ),
          (int) ( g1*f + g2*(1-f) ),
          (int) ( b1*f + b2*(1-f) )
        );   
        return buffer;      
    }

    void pop() {
        for ( double theta = 0; theta < 2*M_PI; theta += M_PI/8 ) {
            Agent& v = add_agent(
                "Virus", 
                x() + rad*cos(angle()+theta), 
                y() + rad*sin(angle()+theta), 
                theta, VIRUS_STYLE);
            v.omni_apply_force(400*cos(angle()+theta),400*sin(angle()+theta));
        }
        for ( int vid : virus_ids ) {
            if ( agent_exists(vid) ) {
                remove_agent(vid);
            }
        }
        remove_agent(id());        
    }

    void show_baby_viruses() {
        std::string svg = "<g>";
        char buffer[512];             
        for ( double theta = 0; theta < 2*M_PI; theta += M_PI/8 ) {
            sprintf(buffer,
              "<circle cx=%f cy=%f r=10 fill='%s' stroke='%s' stroke-width='10px' stroke-opacity='0.25'\"></circle>\n",
              rad*cos(theta) + (rand()%2-1.0)/4,
              rad*sin(theta) + (rand()%2-1.0)/4, 
              interpolate_colors(144,238,144,255,144,0).c_str(), 
              interpolate_colors(144,238,144,0,0,0).c_str()
            );
            svg += buffer;
        }
        svg += "</g>";
        decorate(svg);
    }

    json infected_style() {
        char buffer[256];
        double f = 1 - (double) counter / counter_max;
        return {  {"fill", interpolate_colors(144,238,144,255,255,255) }, 
                  {"stroke", "#888"}, 
                  {"strokeWidth", "10px"},
                  {"strokeOpacity", "0.25"}
              };
    }

    const int fmax = 100.0;
    int counter;
    const int counter_max = 100;
    bool infected;
    vector<int> virus_ids;
    const double rad = 80;

};

class Cell : public Agent {
    public:
    Cell(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    CellController c;
};

DECLARE_INTERFACE(Cell)

#endif