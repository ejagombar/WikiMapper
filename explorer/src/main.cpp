#include <httplib.h>
#include <iostream>
#include <json/json.h> // Include the JSON for Modern C++ header
#include <string>

int main() {
    httplib::Client cli("http://localhost:7474");

    httplib::Headers headers = {{"Accept", "application/json"}, {"Authorization", "Basic bmVvNGo6dGVzdDEyMzQ="}};

    Json::Value root;
    Json::Value statement;
    Json::Value parameters;

    statement["statement"] = "RETURN 1";
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
