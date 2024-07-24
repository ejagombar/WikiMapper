#include "neo4j.h"
#include <httplib.h>
#include <iostream>
#include <json/json.h> // Include the JSON for Modern C++ header

int main() {
    Neo4jData dbData;
    dbData.auth64 = "bmVvNGo6dGVzdDEyMzQ=";
    dbData.dbName = "neo4j";
    dbData.host = "localhost";
    dbData.port = 7474;

    Neo4j db(dbData);

    std::string start = "blues";
    std::string end = "guitar";

    auto res = db.shortestPath(start, end, 25);

    if (res && res->status == 200) {
        std::cout << "Response status: " << res->status << std::endl;
        std::cout << "Response body: " << res->body << std::endl;
    } else {
        std::cerr << "Request failed with status: " << (res ? res->status : 0) << std::endl;
        if (res) {
            std::cerr << "Error: " << res->body << std::endl;
        }
    }

    return 0;
}
