#include "store.hpp"
#include "../lib/base64.hpp"
#include "logger.hpp"
#include <cstdint>
#include <httplib.h>
#include <memory>
#include <string>
#include <vector>

// ------------------ Neo4J Interface ------------------

Neo4jInterface::Neo4jInterface(const std::string url) { m_httpClient = std::make_unique<httplib::Client>(url); }

bool Neo4jInterface::connected() {
    try {
        const std::string testQuery = "RETURN 1 AS test";
        const json parameters = json::object();
        const json query = {{"statements", {{{"statement", testQuery}, {"parameters", parameters}}}}};

        const uint32_t shortTimeout = 100;
        m_httpClient->set_connection_timeout(std::chrono::milliseconds(shortTimeout));
        m_httpClient->set_read_timeout(std::chrono::milliseconds(shortTimeout));
        m_httpClient->set_write_timeout(std::chrono::milliseconds(shortTimeout));

        auto res = m_httpClient->Post("/db/neo4j/tx/commit", query.dump(), "application/json");

        m_httpClient->set_read_timeout(m_timeout_ms);
        m_httpClient->set_write_timeout(m_timeout_ms);
        m_httpClient->set_connection_timeout(m_timeout_ms);

        if (!res || res->status != httplib::StatusCode::OK_200) {
            globalLogger->info("RETURNED {}", res->status);
            m_connected = false;
            return false;
        }

        json data = json::parse(res->body);
        if (data.contains("errors") && !data["errors"].empty()) {
            globalLogger->info("RETURNED Errors");
            m_connected = false;
            return false;
        }

        m_connected = true;
        return true;
    } catch (...) {
        m_connected = false;
        return false;
    }
}

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

    // Check for the top-level "results" array
    if (!data.contains("results") || !data["results"].is_array())
        return pages;

    try {
        for (const auto &result : data["results"]) {
            if (!result.contains("data") || !result["data"].is_array())
                continue;

            for (const auto &entry : result["data"]) {
                if (!entry.contains("row") || !entry["row"].is_array())
                    continue;

                const auto &row = entry["row"];
                if (row.size() >= 1) {
                    std::string pageName = row[0].value("pageName", "");
                    std::string title = row[0].value("title", "");
                    pages.emplace_back(LinkedPage{pageName, title});
                }
            }
        }
    } catch (const json::exception &e) {
        throw std::runtime_error("Failed to parse node: " + std::string(e.what()));
    }

    return pages;
}

bool Neo4jInterface::Authenticate(const std::string &username, const std::string &password) {
    const std::string basicToken = base64::to_base64(username + ":" + password);
    const httplib::Headers headers = {{"Authorization", "Basic " + basicToken}};

    const uint32_t shortTimeout = 50;
    m_httpClient->set_connection_timeout(std::chrono::milliseconds(shortTimeout));
    m_httpClient->set_read_timeout(std::chrono::milliseconds(shortTimeout));
    m_httpClient->set_write_timeout(std::chrono::milliseconds(shortTimeout));

    const auto res = m_httpClient->Get("/", headers);

    m_httpClient->set_read_timeout(m_timeout_ms);
    m_httpClient->set_write_timeout(m_timeout_ms);
    m_httpClient->set_connection_timeout(m_timeout_ms);

    if (res && res->status == httplib::StatusCode::OK_200) {
        m_httpClient->set_default_headers(headers);
        return true;
    }
    return false;
}

std::vector<LinkedPage> Neo4jInterface::GetLinkedPages(const std::string &pageName) {
    if (!m_connected) {
        return {};
    }

    const std::string cypher = "MATCH (:PAGE {pageName: $name})-[]->(related:PAGE) "
                               "RETURN related";

    try {
        json data = ExecuteCypherQuery(cypher, {{"name", pageName}});
        globalLogger->error("{} ", data.dump());
        return ParsePagesFromResult(data);
    } catch (const std::exception &e) {
        throw std::runtime_error("GetLinkedPages failed: " + std::string(e.what()));
        return {};
    }
}

std::vector<LinkedPage> Neo4jInterface::FindShortestPath(const std::string &startPage, const std::string &endPage) {
    if (!m_connected) {
        return {};
    }

    const std::string cypher =
        "MATCH path = shortestPath((start:PAGE {pageName: $startName})-[*]-(end:PAGE {pageName: $endName})) "
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
    if (!m_connected) {
        return {};
    }

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
    if (!m_connected) {
        return {};
    }

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

std::vector<LinkedPage> HttpParsePagesFromResult(const json &data) {
    std::vector<LinkedPage> pages;

    if (!data.is_array())
        return pages;

    try {
        for (const auto &node : data) {
            pages.emplace_back(LinkedPage{node["pageName"].get<std::string>(), node["title"].get<std::string>()});
        }
    } catch (const json::exception &e) {
        throw std::runtime_error("Failed to parse node: " + std::string(e.what()));
    }

    return pages;
}

HttpInterface::HttpInterface(const std::string domain) {
    std::string dom = domain;
    if (dom.ends_with("/")) {
        dom = dom.substr(0, dom.size() - 2);
    }

    m_httpClient = std::make_unique<httplib::Client>(dom);
}

json HttpInterface::GetHttpResults(const std::string &endpoint) {
    const uint32_t shortTimeout = 100;
    m_httpClient->set_connection_timeout(std::chrono::milliseconds(shortTimeout));
    m_httpClient->set_read_timeout(std::chrono::milliseconds(shortTimeout));
    m_httpClient->set_write_timeout(std::chrono::milliseconds(shortTimeout));

    auto res = m_httpClient->Get(endpoint);

    m_httpClient->set_read_timeout(m_timeout_ms);
    m_httpClient->set_write_timeout(m_timeout_ms);
    m_httpClient->set_connection_timeout(m_timeout_ms);

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
    if (!m_connected) {
        return {};
    }

    try {
        return HttpParsePagesFromResult(GetHttpResults("/linked-pages/" + pageName));
    } catch (const std::exception &e) {
        throw std::runtime_error("GetLinkedPages failed for '" + pageName + "': " + std::string(e.what()));
        return {};
    }
}

std::vector<LinkedPage> HttpInterface::GetLinkingPages(const std::string &pageName) {
    if (!m_connected) {
        return {};
    }

    try {
        return HttpParsePagesFromResult(GetHttpResults("/linking-pages/" + pageName));
    } catch (const std::exception &e) {
        throw std::runtime_error("GetLinkingPages failed for '" + pageName + "': " + std::string(e.what()));
        return {};
    }
}

std::vector<LinkedPage> HttpInterface::FindShortestPath(const std::string &startPage, const std::string &endPage) {
    if (!m_connected) {
        return {};
    }

    try {
        return HttpParsePagesFromResult(GetHttpResults("/shortest-path?start=" + startPage + "&end=" + endPage));
    } catch (const std::exception &e) {
        throw std::runtime_error("FindShortestPath failed for '" + startPage + " to " + endPage +
                                 "': " + std::string(e.what()));
        return {};
    }
}

std::vector<LinkedPage> HttpInterface::GetRandomPages(uint32_t count) {
    if (!m_connected) {
        return {};
    }

    try {
        return HttpParsePagesFromResult(GetHttpResults("/random-pages/" + std::to_string(count)));
    } catch (const std::exception &e) {
        throw std::runtime_error("GetRandomPages failed : " + std::string(e.what()));
        return {};
    }
}

bool HttpInterface::connected() {
    try {
        json result = GetHttpResults("/connected");

        if (result.contains("connected") && result["connected"].is_boolean()) {
            m_connected = result["connected"].get<bool>();
        } else {
            m_connected = false;
        }
    } catch (...) {
        m_connected = false;
    }

    return m_connected;
}
