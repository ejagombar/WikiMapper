#include "store.hpp"
#include "../lib/base64.hpp"
#include "../lib/json.hpp"
#include <httplib.h>
#include <memory>
#include <string>
#include <vector>

using json = nlohmann::json;

Neo4jInterface::Neo4jInterface(const std::string url) { m_httpClient = std::make_unique<httplib::Client>(url); }

bool Neo4jInterface::Authenticate(const std::string username, const std::string password) {
    const std::string basicToken = base64::to_base64(username + ":" + password);

    const httplib::Headers headers = {{"Authorization", "Basic " + basicToken}};
    const httplib::Result res = m_httpClient->Get("/", headers);

    if (res->status == httplib::StatusCode::OK_200) {
        m_httpClient->set_default_headers(headers);
        return true;
    }
    return false;
}

std::vector<LinkedPage> Neo4jInterface::GetLinkedPages(const std::string pageName) {
    const std::string cypherQuery =
        R"( { "statements": [ { "statement": "MATCH (:PAGE {pageName: $name})-[]->(related:PAGE) RETURN related", "parameters": { "name": ")" +
        pageName + R"(" } } ] })";

    std::vector<LinkedPage> linkedPages;

    httplib::Result res = m_httpClient->Post("/db/neo4j/tx/commit", cypherQuery, "application/json");

    if (res && res->status != httplib::StatusCode::OK_200) {
        std::cerr << "Failed to fetch nodes. Status: " << (res ? res->status : -1)
                  << ". Error: " << (res ? res->body : "No response") << std::endl;
        return linkedPages;
    }

    json data = json::parse(res->body);

    const auto &result = data["results"][0];
    const auto rows = result["data"];

    linkedPages.reserve(rows.size());

    for (const auto &row : rows) {
        const auto &x = row["row"][0];
        linkedPages.emplace_back(LinkedPage{x["pageName"], x["title"]});
    }

    return linkedPages;
}
