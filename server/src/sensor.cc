#include <math.h>
#include "enviro.h"

cpVect rotate(cpVect v, cpFloat theta) {
    return {
        x: cos(theta) * v.x - sin(theta) * v.y,
        y: sin(theta) * v.x + cos(theta) * v.y
    };
}

std::pair<double,std::string> RangeSensor::value() {

    double distance = 10000;
    std::string reflection_type = "None";

    World * world = _agent_ptr->get_world_ptr();

    double theta = _agent_ptr->angle();
    cpVect P = _agent_ptr->position();

    cpVect Lrot = rotate(_location, theta);
    cpVect start =  cpvadd(P, Lrot);
    cpFloat angle = _angle + _agent_ptr->angle();              
    cpVect end = cpvadd(start, { x: 1000 * cos(angle), y: 1000 * sin(angle)});       

    world->all([&](Agent& other ) {

        if ( &other != _agent_ptr && 
             other.definition()["type"] != "noninteractive" &&
             other.definition()["type"] != "invisible" ) {

            cpSegmentQueryInfo info;        
            cpShape * shape = other.get_shape();
            cpShapeSegmentQuery(shape, start, end, 0, &info);

            if ( info.shape != NULL ) {
                double d = cpvdist(start, info.point);
                if ( d < distance ) {
                    distance = d;
                    reflection_type = other.name();
                }
            }
        }
        
    });

    return std::make_pair(distance,reflection_type);

}