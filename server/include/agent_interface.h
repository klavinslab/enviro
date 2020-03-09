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
        double x();
        double y();
        cpVect velocity();
        double vx();
        double vy();
        double angle();
        double angular_velocity();
        int id();

        // Actuators
        void apply_force(double thrust, double torque);
        void track_velocity(double linear_velocity, double angular_velocity, 
                            double kL=10, double kR=10);
        void damp_movement();  
        void move_toward(double x, double y, double vF=75, double vR=20);
        void teleport(double x, double y, double theta);
        
        void omni_apply_force(double fx, double fy);
        void omni_damp_movement();  
        void omni_track_velocity(double vx, double vy, double k=10); 
        void omni_move_toward(double x, double y, double v=1);
        
        // Sensors
        double sensor_value(int index);
        std::vector<double> sensor_values();

        // Collisons
        void notice_collisions_with(const std::string agent_type, std::function<void(Event&)> handler);
        void ignore_collisions_with(const std::string agent_type);

        // Constraints
        void attach_to(Agent &other_agent);

        // Agent management
        Agent& find_agent(int id);
        bool agent_exists(int id);
        void remove_agent(int id);
        Agent& add_agent(const std::string name, double x, double y, double theta, const json style);

        // Styles
        void set_style(json style);

        virtual ~AgentInterface() {} // needed to make dynamic_cast work

        protected:
        Agent * agent;

    };

}

#endif