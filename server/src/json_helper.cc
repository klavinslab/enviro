#include <fstream>
#include <iostream>
#include <exception>
#include "json/json.h"

using nlohmann::json;

namespace json_helper {

    json read(std::string filename) {

        std::ifstream ifs(filename);
        if ( ifs.fail() ) {
            std::cerr << "Could not open json file " 
                      << filename 
                      << "\n";
            throw std::runtime_error("file error");
        }

        json object;
        ifs >> object;
        return object;

    }

    void check(json object, json schema) {

        if ( schema.is_null() ) {
            if ( ! object.is_null() ) {
                throw std::runtime_error("JSON Error: Expected null but got " + object.dump() );
            }
        } else if ( schema.is_boolean() ) {
            if ( ! object.is_boolean() ) {
                throw std::runtime_error("JSON Error: Expected Boolean but got " + object.dump() );
            }
        } else if ( schema.is_number() ) {
            if ( ! object.is_number() ) {
                throw std::runtime_error("JSON Error: Expected number but got " + object.dump() );
            }
        } else if ( schema.is_string() ) {
            if ( ! object.is_string() ) {
                throw std::runtime_error("JSON Error: Expected string but got " + object.dump());
            }
        } else if ( schema.is_array() ) {
            if ( schema.size() < 1 ) {
                throw std::runtime_error("JSON Error: Expected at least one element in schema array");
            }
            if ( object.is_array() ) {
                for (auto& element : object) {
                    check(element, schema[0]);
                }
            } else {
                throw std::runtime_error("JSON Error: Expected array but got " + object.dump());
            }
        } else if ( schema.is_object() ) {
            if ( object.is_object() ) {
                for (auto& [key, value] : schema.items()) {
                    if ( object.find(key) == object.end() ) {
                        throw std::runtime_error("JSON Error: Expected key '" + key + "' in object " + object.dump());
                    } else {
                        check(object[key], value);
                    }
                }
            } else {
                throw std::runtime_error("JSON Error: Expected object but got " + object.dump());
            }
        } 

    }

};