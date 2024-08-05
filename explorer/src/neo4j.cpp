#include "neo4j.h"
#include <httplib.h>
#include <json/json.h>
#include <string>

// int main() {
//     Neo4jData dbData;
//     dbData.auth64 = "bmVvNGo6dGVzdDEyMzQ=";
//     dbData.dbName = "neo4j";
//     dbData.host = "localhost";
//     dbData.port = 7474;
//
//     Neo4j db(dbData);
//
//     std::string start = "blues";
//     std::string end = "guitar";
//
//     auto res = db.shortestPath(start, end, 25);
//
//     if (res && res->status == 200) {
//         // std::cout << "Response status: " << res->status << std::endl;
//         // std::cout << "Response body: " << res->body << std::endl;
//
//         std::cout << res->body << std::endl;
//     } else {
//         std::cerr << "Request failed with status: " << (res ? res->status : 0) << std::endl;
//         if (res) {
//             std::cerr << "Error: " << res->body << std::endl;
//         }
//     }
//
//     return 0;
// }

Neo4j::Neo4j(const Neo4jData in)
    : m_headers({{"Accept", "application/json"}, {"Authorization", "Basic " + in.auth64}}),
      m_path("/db/" + in.dbName + "/tx/commit"),
      cli("http://" + in.host + ":" + std::to_string(in.port)) {}

httplib::Result Neo4j::shortestPath(std::string &start, std::string &end, int timeout) {
    cli.set_connection_timeout(timeout, 0);
    cli.set_read_timeout(timeout, 0);
    cli.set_write_timeout(timeout, 0);

    Json::Value root;
    Json::Value statement;
    Json::Value parameters;

    parameters["nameStart"] = start;
    parameters["nameEnd"] = end;

    statement["parameters"] = parameters;
    statement["statement"] =
        " MATCH p = shortestPath((start {pageName:$nameStart})-[:LINK*1..8]->(end "
        "{pageName:$nameEnd})) RETURN p";

    root["statements"].append(statement);

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";

    std::string json_payload = Json::writeString(writer, root);

    return cli.Post(m_path, m_headers, json_payload, "application/json");
}
