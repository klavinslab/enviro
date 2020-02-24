#include <math.h>
#include "enviro.h"

cpVect rotate(cpVect v, cpFloat theta) {
    return {
        x: cos(theta) * v.x - sin(theta) * v.y,
        y: sin(theta) * v.x + cos(theta) * v.y
    };
}

double RangeSensor::value() {

    double distance = 10000;
    World * world = _agent_ptr->get_world_ptr();
    // std::cout << "finding range sensor value\n";

    double theta = _agent_ptr->angle();
    cpVect P = _agent_ptr->position();

    cpVect Lrot = rotate(_location, theta);
    cpVect start =  cpvadd(P, Lrot);
    cpFloat angle = _angle + _agent_ptr->angle();              
    cpVect end = cpvadd(start, { x: 1000 * cos(angle), y: 1000 * sin(angle)});       

    world->all([this, &distance, &start, &angle, &end](Agent& other) {

        if ( &other != _agent_ptr ) {

            cpSegmentQueryInfo info;        
            cpShape * shape = other.get_shape();
            cpShapeSegmentQuery(shape, start, end, 0, &info);

            if ( info.shape != NULL ) {
                double d = cpvdist(start, info.point);
                // std::cout << "found object in sensor path\n" 
                //           "    agent pos = (" << _agent_ptr->position().x << ", " << _agent_ptr->position().y << ")\n" 
                //           "    agent angle = " << _agent_ptr->angle() << "\n"
                //           "    info.point = (" << info.point.x << ", " << info.point.y << ")\n" 
                //           "    other.name = " << other.name() << "\n"
                //           "    distance = " << d << "\n"
                //           "    start = (" << start.x << ", " << start.y << ")\n"
                //           "    angle = " << angle << "\n"
                //           "    end = (" << end.x << ", " << end.y << ")\n"
                //           "    info.alpha = " << info.alpha << "\n";
                if ( d < distance ) {
                    distance = d;
                }
            }
        }
        
    });

    return distance;

}