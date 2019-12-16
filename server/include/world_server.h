#ifndef __ENVIRO_SERVER__H
#define __ENVIRO_SERVER__H

#include <iostream>
#include <ctime>
#include <map>
#include <string>
#include <mutex>

#include "httplib/httplib.h"
#include "json/json.h"
#include "agent.h"
#include "world.h"

using namespace httplib;
using nlohmann::json; 

namespace enviro {

    long int unix_timestamp(); 
    class World;

    class WorldServer {

        public:
        WorldServer(World& world, std::mutex& mutex, const char* ip, int port);
        void run();
        void stop();
        void get_state(const Request& req, Response& res);

        private:
        Server server;
        World& world;
        std::mutex& manager_mutex;
        const char* ip;
        int port;

    };

}

#endif