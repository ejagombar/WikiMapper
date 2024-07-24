#include <httplib.h>
#include <iostream>
#include <json/json.h> // Include the JSON for Modern C++ header
#include <string>

int main() {
    httplib::Client cli("http://localhost:7474");
    cli.set_connection_timeout(25, 0);

    cli.set_read_timeout(25, 0);
    cli.set_write_timeout(25, 0);

    httplib::Headers headers = {{"Accept", "application/json"}, {"Authorization", "Basic bmVvNGo6dGVzdDEyMzQ="}};

    Json::Value root;
    Json::Value statement;
    Json::Value parameters;

    statement["statement"] = "MATCH (start {pageName: $nameEnd}), (end {pageName:$nameStart}), p = "
                             "shortestPath((start)-[:LINK*1..8]->(end)) RETURN p";

    parameters["upperBound"] = "8";
    parameters["nameStart"] = "hitler";
    parameters["nameEnd"] = "jazz";

    statement["parameters"] = parameters;

    root["statements"].append(statement);

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";

    std::string json_payload = Json::writeString(writer, root);
    std::cout << json_payload << std::endl;

    auto res = cli.Post("/db/neo4j/tx/commit", headers, json_payload, "application/json");

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
