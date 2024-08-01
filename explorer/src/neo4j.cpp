#include "neo4j.h"
#include <httplib.h>
#include <json/json.h>
#include <string>

Neo4j::Neo4j(const Neo4jData in)
    : m_headers({{"Accept", "application/json"}, {"Authorization", "Basic " + in.auth64}}),
      m_path("/db/" + in.dbName + "/tx/commit"), cli("http://" + in.host + ":" + std::to_string(in.port)) {}

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
    statement["statement"] = " MATCH p = shortestPath((start {pageName:$nameStart})-[:LINK*1..8]->(end "
                             "{pageName:$nameEnd})) RETURN p";

    root["statements"].append(statement);

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";

    std::string json_payload = Json::writeString(writer, root);

    return cli.Post(m_path, m_headers, json_payload, "application/json");
}
