#ifndef __ENVIRO_SCHEMA__H
#define __ENVIRO_SCHEMA__H

#include "json/json.h"

using nlohmann::json; 

json ENVIRO_CONFIG_SCHEMA = R"(
    {
        "name": "",
        "ip": "",
        "port": 0,
        "agents": [
            {
                "definition": "",
                "position": {
                    "x": 0, 
                    "y": 0, 
                    "theta": 0
                }
            }
        ]
    }
)"_json;

json ENVIRO_AGENT_SCHEMA = R"(
    {
        "name": "",
        "type": "",
        "description": "",
        "shape": [
            { "x": 0, "y": 0 }
        ],
        "friction": {
            "collision": 0,
            "linear": 0,
            "rotational": 0
        },
        "mass": 0,    
        "controller": ""
    }
)"_json;

#endif