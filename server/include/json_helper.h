#ifndef __ENVIRO_JSON_HELPER__H
#define __ENVIRO_JSON_HELPER__H

#include "json/json.h"

using nlohmann::json;

namespace json_helper {

    json read(std::string filename);
    bool check(json object, json schema);

};

#endif