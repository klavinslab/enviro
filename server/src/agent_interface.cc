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

double AgentInterface::x() {
  ASSERT_AGENT_EXISTS("x");
  return agent->x();
}

double AgentInterface::y() {
  ASSERT_AGENT_EXISTS("y");
  return agent->y();
}

cpVect AgentInterface::velocity() {
  ASSERT_AGENT_EXISTS("velocity");
  return agent->velocity();
}

double AgentInterface::vx() {
  ASSERT_AGENT_EXISTS("vx");
  return agent->vx();
}

double AgentInterface::vy() {
  ASSERT_AGENT_EXISTS("vy");
  return agent->vy();
}

double AgentInterface::angle() {
  ASSERT_AGENT_EXISTS("angle");
  return agent->angle();
}

double AgentInterface::angular_velocity() {
  ASSERT_AGENT_EXISTS("angular_velocity");
  return agent->angular_velocity();
}

// Actuators

void AgentInterface::omni_apply_force(double fx, double fy) {
    ASSERT_AGENT_EXISTS("omni_apply_force");
    agent->omni_apply_force(fx, fy);  
}

void AgentInterface::apply_force(double thrust, double torque) {
    ASSERT_AGENT_EXISTS("apply_force");
    agent->apply_force(thrust, torque);
}

void AgentInterface::track_velocity(double linear_velocity, double angular_velocity,
                                    double kL, double kR) {
    ASSERT_AGENT_EXISTS("track_velocity");
    agent->track_velocity(linear_velocity, angular_velocity, kL, kR);
}

void AgentInterface::omni_track_velocity(double vx, double vy, double k) {
    ASSERT_AGENT_EXISTS("omni_track_velocity");
    agent->omni_track_velocity(vx, vy, k);
}

void AgentInterface::damp_movement() {
    ASSERT_AGENT_EXISTS("damp_movement");
    agent->damp_movement();
}   

void AgentInterface::omni_damp_movement() {
    ASSERT_AGENT_EXISTS("omni_damp_movement");
    agent->omni_damp_movement();
}   

void AgentInterface::teleport(double x, double y, double theta) {
    ASSERT_AGENT_EXISTS("teleport");
    agent->teleport(x,y,theta);
}   

void AgentInterface::move_toward(double x, double y, double vF, double vR) {
    ASSERT_AGENT_EXISTS("move_toward");
    agent->move_toward(x,y,vF,vR);  
}

void AgentInterface::omni_move_toward(double x, double y, double v) {
    ASSERT_AGENT_EXISTS("omni_move_toward");
    agent->omni_move_toward(x,y,v);
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