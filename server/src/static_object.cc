#include "enviro.h"

using namespace enviro;

StaticObject::StaticObject(json spec, World& world) : Agent(spec, world) {
    std::cout << "Made new static object\n";
}

json StaticObject::build_specification(json static_entry) {

    json result = R"( { "definition": "none" } )"_json;

    result["definition"] = R"(
        {
            "name": "StaticObject",
            "type": "static",
            "description": "A Static Object",
            "friction": {
                "collision": 10,
                "linear": 10,
                "rotational": 100
            },
            "mass": 1
        }    
    )"_json;

    result["definition"]["shape"] = static_entry["shape"];
    result["position"] = R"({"x": 0, "y": 0, "theta": 0})"_json;

    return result;

}