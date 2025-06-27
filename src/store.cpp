#include "store.hpp"
#include "../lib/base64.hpp"
#include <cstdint>
#include <httplib.h>
#include <memory>
#include <string>
#include <vector>

// ------------------ Neo4J Interface ------------------

Neo4jInterface::Neo4jInterface(const std::string url) { m_httpClient = std::make_unique<httplib::Client>(url); }

json Neo4jInterface::ExecuteCypherQuery(const std::string &cypher, const json &parameters) {
    const json query = {{"statements", {{{"statement", cypher}, {"parameters", parameters}}}}};

    httplib::Result res = m_httpClient->Post("/db/neo4j/tx/commit", query.dump(), "application/json");

    if (!res) {
        throw std::runtime_error("No response from server");
    }

    if (res->status != httplib::StatusCode::OK_200) {
        throw std::runtime_error("HTTP error " + std::to_string(res->status) + ": " + res->body);
    }

    json data;
    try {
        data = json::parse(res->body);
    } catch (const json::parse_error &e) {
        throw std::runtime_error("Failed to parse JSON response: " + std::string(e.what()));
    }

    if (data.contains("errors") && !data["errors"].empty()) {
        std::string errorMsg = "Cypher errors:";
        for (const auto &error : data["errors"]) {
            errorMsg += "\n[" + error["code"].get<std::string>() + "] " + error["message"].get<std::string>();
        }
        throw std::runtime_error(errorMsg);
    }

    return data;
}

std::vector<LinkedPage> ParsePagesFromResult(const json &data) {
    std::vector<LinkedPage> pages;

    if (!data.contains("results") || data["results"].empty())
        return pages;
    const auto &result = data["results"][0];

    if (!result.contains("data") || !result["data"].is_array())
        return pages;
    const auto &rows = result["data"];

    pages.reserve(rows.size());

    try {
        for (const auto &row : rows) {
            const auto &node = row["row"][0];
            pages.emplace_back(LinkedPage{node["pageName"].get<std::string>(), node["title"].get<std::string>()});
        }
    } catch (const json::exception &e) {
        throw std::runtime_error("Failed to parse node: " + std::string(e.what()));
    }

    return pages;
}

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

std::vector<LinkedPage> Neo4jInterface::GetLinkedPages(const std::string &pageName) {
    const std::string cypher = "MATCH (:PAGE {pageName: $name})-[]->(related:PAGE) "
                               "RETURN related";

    try {
        json data = ExecuteCypherQuery(cypher, {{"name", pageName}});
        return ParsePagesFromResult(data);
    } catch (const std::exception &e) {
        throw std::runtime_error("GetLinkedPages failed: " + std::string(e.what()));
        return {};
    }
}

std::vector<LinkedPage> Neo4jInterface::FindShortestPath(const std::string &startPage, const std::string &endPage) {
    const std::string cypher = "MATCH path = shortestPath((start:PAGE {pageName: $startName})-[*]-(end:PAGE {pageName: $endName})) "
                               "RETURN nodes(path) AS nodes";

    try {
        json data = ExecuteCypherQuery(cypher, {{"startName", startPage}, {"endName", endPage}});

        std::vector<LinkedPage> pathNodes;
        const auto &nodes = data["results"][0]["data"][0]["row"][0];
        for (const auto &node : nodes) {
            pathNodes.emplace_back(LinkedPage{node["pageName"].get<std::string>(), node["title"].get<std::string>()});
        }
        return pathNodes;
    } catch (const std::exception &e) {
        throw std::runtime_error("FindShortestPath failed: " + std::string(e.what()));
        return {};
    }
}

std::vector<LinkedPage> Neo4jInterface::GetLinkingPages(const std::string &pageName) {
    const std::string cypher = "MATCH (related:PAGE)-[r]->(:PAGE {pageName: $name}) "
                               "RETURN related";

    try {
        json parameters = {{"name", pageName}};
        json response = ExecuteCypherQuery(cypher, parameters);
        return ParsePagesFromResult(response);
    } catch (const std::exception &e) {
        throw std::runtime_error("GetLinkingPages failed for '" + pageName + "': " + std::string(e.what()));
        return {};
    }
}

std::vector<LinkedPage> Neo4jInterface::GetRandomPages(uint32_t count) {
    const std::string cypher = "MATCH (p:PAGE) "
                               "WITH p, RAND() AS r "
                               "ORDER BY r "
                               "RETURN p "
                               "LIMIT $limit";

    try {
        json parameters = {{"limit", count}};
        json response = ExecuteCypherQuery(cypher, parameters);
        return ParsePagesFromResult(response);
    } catch (const std::exception &e) {
        throw std::runtime_error("GetRandomPages failed: " + std::string(e.what()));
        return {};
    }
}

// ------------------ HTTP Interface ------------------

HttpInterface::HttpInterface(const std::string domain) {
    std::string dom = domain;
    if (dom.ends_with("/")) {
        dom = dom.substr(0, dom.size() - 2);
    }

    m_httpClient = std::make_unique<httplib::Client>(dom);
}

json HttpInterface::GetHttpResults(const std::string &endpoint) {
    const httplib::Result res = m_httpClient->Get(endpoint);

    if (!res) {
        throw std::runtime_error("No response from server");
    }

    if (res->status != httplib::StatusCode::OK_200) {
        throw std::runtime_error("HTTP error " + std::to_string(res->status) + ": " + res->body);
    }

    json data;
    try {
        data = json::parse(res->body);
    } catch (const json::parse_error &e) {
        throw std::runtime_error("Failed to parse JSON response: " + std::string(e.what()));
    }

    if (data.contains("errors") && !data["errors"].empty()) {
        std::string errorMsg = "Cypher errors:";
        for (const auto &error : data["errors"]) {
            errorMsg += "\n[" + error["code"].get<std::string>() + "] " + error["message"].get<std::string>();
        }
        throw std::runtime_error(errorMsg);
    }

    return data;
}

std::vector<LinkedPage> HttpInterface::GetLinkedPages(const std::string &pageName) {
    try {
        return ParsePagesFromResult(GetHttpResults("/linked-pages/" + pageName));
    } catch (const std::exception &e) {
        throw std::runtime_error("GetLinkedPages failed for '" + pageName + "': " + std::string(e.what()));
        return {};
    }
}

std::vector<LinkedPage> HttpInterface::GetLinkingPages(const std::string &pageName) {
    try {
        return ParsePagesFromResult(GetHttpResults("/linking-pages/" + pageName));
    } catch (const std::exception &e) {
        throw std::runtime_error("GetLinkingPages failed for '" + pageName + "': " + std::string(e.what()));
        return {};
    }
}

std::vector<LinkedPage> HttpInterface::FindShortestPath(const std::string &startPage, const std::string &endPage) {
    try {
        return ParsePagesFromResult(GetHttpResults("/shortest-path?start=" + startPage + "&end=" + endPage));
    } catch (const std::exception &e) {
        throw std::runtime_error("FindShortestPath failed for '" + startPage + " to " + endPage + "': " + std::string(e.what()));
        return {};
    }
}

std::vector<LinkedPage> HttpInterface::GetRandomPages(uint32_t count) {
    try {
        return ParsePagesFromResult(GetHttpResults("/random-pages/" + std::to_string(count)));
    } catch (const std::exception &e) {
        throw std::runtime_error("GetRandomPages failed : " + std::string(e.what()));
        return {};
    }
}
