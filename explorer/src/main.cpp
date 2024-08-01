#include "neo4j.h"
#include <httplib.h>
#include <iostream>
#include <json/json.h>

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

    // if (res && res->status == 200) {
    //     // Parse the JSON response
    //     Json::CharReaderBuilder reader;
    //     Json::Value response_json;
    //     std::string errors;
    //
    //     // Read the JSON response
    //     std::istringstream response_stream(res->body);
    //     if (Json::parseFromStream(reader, response_stream, &response_json, &errors)) {
    //         // Extract the desired data from the JSON response
    //         if (!response_json["results"].empty()) {
    //             const Json::Value &result_data = response_json["results"][0];
    //             if (!result_data["data"].empty()) {
    //                 // Extract the data from the response (e.g., the path nodes)
    //                 const Json::Value &path_data = result_data["data"][0]["row"][0];
    //                 std::cout << "Path: " << path_data.toStyledString() << std::endl;
    //                 std::cout << path_data.toStyledString(); // Return the path data as a string
    //             }
    //         }
    //         // Return an error if the expected data isn't found
    //         std::cout << "Error: No path found in response.";
    //     } else {
    //         // Log any JSON parsing errors
    //         std::cerr << "JSON Parse Error: " << errors << std::endl;
    //         std::cout << "Error: Failed to parse JSON response.";
    //     }
    // } else {
    //     // Log HTTP request errors
    //     std::cerr << "HTTP Error: " << (res ? res->status : -1) << " " << (res ? res->body : "No response")
    //               << std::endl;
    //     std::cout << "Error: Unable to reach Neo4j server.";
    // }

    if (res && res->status == 200) {
        // std::cout << "Response status: " << res->status << std::endl;
        // std::cout << "Response body: " << res->body << std::endl;

        std::cout << res->body << std::endl;
    } else {
        std::cerr << "Request failed with status: " << (res ? res->status : 0) << std::endl;
        if (res) {
            std::cerr << "Error: " << res->body << std::endl;
        }
    }

    return 0;
}
