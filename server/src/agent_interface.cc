#include "agent_interface.h"

#define ASSERT_AGENT_EXISTS(__name__) \
  if ( agent == NULL ) { \
      std::string msg = std::string("Agent pointer is null in call to ") \
                      + __name__ + "(). " \
                      + "Agent interface may have been used before call to add_process"; \
      throw Exception(msg); \
} 

int AgentInterface::id() {
  ASSERT_AGENT_EXISTS("id");
  return agent->get_id();
}

// State
cpVect AgentInterface::position() {
  ASSERT_AGENT_EXISTS("position");
  return agent->position();
}

cpVect AgentInterface::velocity() {
  ASSERT_AGENT_EXISTS("velocity");
  return agent->velocity();
}

cpFloat AgentInterface::angle() {
  ASSERT_AGENT_EXISTS("angle");
  return agent->angle();
}

cpFloat AgentInterface::angular_velocity() {
  ASSERT_AGENT_EXISTS("angular_velocity");
  return agent->angular_velocity();
}

// Actuators
void AgentInterface::apply_force(double thrust, double torque) {
    ASSERT_AGENT_EXISTS("apply_force");
    agent->apply_force(thrust, torque);
}

void AgentInterface::track_velocity(double linear_velocity, double angular_velocity,
                                    double kL, double kR) {
    ASSERT_AGENT_EXISTS("track_velocity");
    agent->track_velocity(linear_velocity, angular_velocity, kL, kR);
}

void AgentInterface::damp_movement() {
    ASSERT_AGENT_EXISTS("damp_movement");
    agent->damp_movement();
}   

void AgentInterface::teleport(double x, double y, double theta) {
    ASSERT_AGENT_EXISTS("teleport");
    agent->teleport(x,y,theta);
}   

void AgentInterface::move_toward(double x, double y) {
    ASSERT_AGENT_EXISTS("move_toward");
    agent->move_toward(x,y);  
}

// Sensors
double AgentInterface::sensor_value(int index) {
    ASSERT_AGENT_EXISTS("sensor_value");
    return agent->sensor_value(index);
}

std::vector<double> AgentInterface::sensor_values() {
    ASSERT_AGENT_EXISTS("sensor_values");
    return agent->sensor_values();
}      