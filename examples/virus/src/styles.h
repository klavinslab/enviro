#ifndef __VIRUS_STYLES__H
#define __VIRUS_STYLES__H

#include "enviro.h"

using namespace enviro;

const json  VIRUS_STYLE = { 
                {"fill", "orange"}, 
                {"stroke", "black"}, 
                {"strokeWidth", "10px"},
                {"strokeOpacity", "0.25"}
            },           
            VIRUS_FRAGMENT_STYLE = { 
                {"fill", "orange"}, 
                {"stroke", "black"}, 
                {"strokeWidth", "5px"},
                {"strokeOpacity", "0.25"}
            },
            BULLET_STYLE = { 
                   {"fill", "green"}, 
                   {"stroke", "#888"}, 
                   {"strokeWidth", "2px"},
                   {"strokeOpacity", "0.25"}
               };

#endif