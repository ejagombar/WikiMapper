#include "interface.hpp"
#include "../lib/base64.hpp"
#include "logger.hpp"
#include <cstdint>
#include <httplib.h>
#include <memory>
#include <string>
#include <vector>

GraphUpdateData ParseGraphResult(const json &data) {
    GraphUpdateData update;

    if (!data.contains("results") || !data["results"].is_array())
        return update;

    try {
        for (const auto &result : data["results"]) {
            for (const auto &entry : result["data"]) {
                const auto &row = entry["row"];
                // We assume the Cypher query always returns [sourceName, sourceTitle, targetName, targetTitle]
                // or specific subsets. This parsing depends on the specific query columns below.

                // Example row mapping based on "GetLocalSubgraph" query below:
                // row[0] = sourceNode, row[1] = targetNode

                if (row.size() >= 2) {
                    std::string srcName = row[0]["pageName"];
                    std::string srcTitle = row[0]["title"];
                    std::string tgtName = row[1]["pageName"];
                    std::string tgtTitle = row[1]["title"];

                    update.nodes.push_back({srcName, srcTitle});
                    update.nodes.push_back({tgtName, tgtTitle});
                    update.edges.push_back({srcName, tgtName});
                }
            }
        }
    } catch (const std::exception &e) {
        globalLogger->error("Parse Error: {}", e.what());
    }
    return update;
}

std::vector<NodeData> ParseGenericNodeResult(const json &data) {
    std::vector<NodeData> results;

    if (!data.contains("results") || !data["results"].is_array())
        return results;

    try {
        for (const auto &result : data["results"]) {
            for (const auto &entry : result["data"]) {
                const auto &row = entry["row"];
                // Expecting row[0] = pageName, row[1] = title
                if (row.size() >= 2) {
                    results.emplace_back(NodeData{row[0].get<std::string>(), row[1].get<std::string>()});
                }
            }
        }
    } catch (const std::exception &e) {
        globalLogger->error("JSON Parse Error: {}", e.what());
    }

    return results;
}

// ------------------ Neo4J Interface ------------------

void setClientTimeoutMs(std::unique_ptr<httplib::Client> &m_httpClient, uint32_t timeout) {
    m_httpClient->set_read_timeout(std::chrono::milliseconds(timeout));
    m_httpClient->set_write_timeout(std::chrono::milliseconds(timeout));
    m_httpClient->set_connection_timeout(std::chrono::milliseconds(timeout));
}

Neo4jInterface::Neo4jInterface(const std::string url) { m_httpClient = std::make_unique<httplib::Client>(url); }

Neo4jInterface::~Neo4jInterface() {};

bool Neo4jInterface::connected() {
    try {
        const std::string testQuery = "RETURN 1 AS test";
        const json parameters = json::object();
        const json query = {{"statements", {{{"statement", testQuery}, {"parameters", parameters}}}}};

        setClientTimeoutMs(m_httpClient, 100);
        auto res = m_httpClient->Post("/db/neo4j/tx/commit", query.dump(), "application/json");
        setClientTimeoutMs(m_httpClient, m_timeout_ms);

        if (!res) {
            globalLogger->info("RETURNED (no response from Neo4j)");
            m_connected = false;
            return false;
        }

        if (res->status != httplib::StatusCode::OK_200) {
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

std::vector<NodeData> ParsePagesFromResult(const json &data) {
    std::vector<NodeData> pages;

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
                    pages.emplace_back(NodeData{pageName, title});
                }
            }
        }
    } catch (const json::exception &e) {
        throw std::runtime_error("Failed to parse node: " + std::string(e.what()));
    }

    return pages;
}

// Gets the center node and all its neighbors and the relationships
GraphUpdateData Neo4jInterface::GetLocalSubgraph(const std::string &centerPageName, int limit) {
    if (!m_connected)
        return {};

    const std::string cypher = limit > 0 ? "MATCH (n:PAGE)-[r]-(m:PAGE) "
                                           "WHERE n.pageName = toLower($name) "
                                           "RETURN {pageName: n.pageName, title: n.title} as source, "
                                           "       {pageName: m.pageName, title: m.title} as target "
                                           "LIMIT $limit"
                                         : "MATCH (n:PAGE)-[r]-(m:PAGE) "
                                           "WHERE n.pageName = toLower($name) "
                                           "RETURN {pageName: n.pageName, title: n.title} as source, "
                                           "       {pageName: m.pageName, title: m.title} as target";

    try {
        json params = {{"name", centerPageName}};
        if (limit > 0)
            params["limit"] = limit;
        json data = ExecuteCypherQuery(cypher, params);
        return ParseGraphResult(data);
    } catch (...) {
        globalLogger->error("Failed to run local subgraph query for '{}'", centerPageName);
        return {};
    }
}

// Find all the links between the given nodes
GraphUpdateData Neo4jInterface::GetInterconnections(const std::vector<std::string> &activeNodeNames, int limit) {
    if (!m_connected || activeNodeNames.empty())
        return {};

    // UNWIND lets each lookup hit the pageName index instead of doing a full edge scan.
    const std::string cypher = limit > 0 ? "UNWIND $names AS name "
                                           "MATCH (a:PAGE {pageName: name})-[]->(b:PAGE) "
                                           "WHERE b.pageName IN $names "
                                           "RETURN {pageName: a.pageName, title: a.title} as source, "
                                           "       {pageName: b.pageName, title: b.title} as target "
                                           "LIMIT $limit"
                                         : "UNWIND $names AS name "
                                           "MATCH (a:PAGE {pageName: name})-[]->(b:PAGE) "
                                           "WHERE b.pageName IN $names "
                                           "RETURN {pageName: a.pageName, title: a.title} as source, "
                                           "       {pageName: b.pageName, title: b.title} as target";

    try {
        json params = {{"names", activeNodeNames}};
        if (limit > 0)
            params["limit"] = limit;
        json data = ExecuteCypherQuery(cypher, params);
        return ParseGraphResult(data);
    } catch (...) {
        globalLogger->error("Failed to run GetInterconnections ({} names)", activeNodeNames.size());
        return {};
    }
}

bool Neo4jInterface::Authenticate(const std::string &username, const std::string &password) {
    const std::string basicToken = base64::to_base64(username + ":" + password);
    const httplib::Headers headers = {{"Authorization", "Basic " + basicToken}};

    setClientTimeoutMs(m_httpClient, 50);
    const auto res = m_httpClient->Get("/", headers);
    setClientTimeoutMs(m_httpClient, m_timeout_ms);

    if (res && res->status == httplib::StatusCode::OK_200) {
        m_httpClient->set_default_headers(headers);

        // Ensure a range index exists on pageName for fast prefix search
        try {
            ExecuteCypherQuery("CREATE INDEX page_pageName_idx IF NOT EXISTS FOR (n:PAGE) ON (n.pageName)", {});
        } catch (const std::exception &e) {
            globalLogger->warn("Could not create pageName index: {}", e.what());
        }

        return true;
    }
    return false;
}

std::vector<NodeData> Neo4jInterface::GetLinkedPages(const std::string &queryString) {
    if (!m_connected) {
        return {};
    }

    const std::string cypher = "MATCH (p:PAGE)-[]->(related:PAGE) "
                               "WHERE p.pageName = $query OR p.title = $query "
                               "RETURN related";

    try {
        json data = ExecuteCypherQuery(cypher, {{"query", queryString}});
        return ParsePagesFromResult(data);
    } catch (const std::exception &e) {
        throw std::runtime_error("GetLinkedPages failed: " + std::string(e.what()));
        return {};
    }
}

std::vector<NodeData> Neo4jInterface::FindShortestPath(const std::string &startPage, const std::string &endPage) {
    if (!m_connected)
        return {};

    const std::string cypher =
        "MATCH path = shortestPath((start:PAGE {pageName: $startName})-[*]-(end:PAGE {pageName: $endName})) "
        "UNWIND nodes(path) AS n "
        "RETURN n.pageName, n.title";

    try {
        json data = ExecuteCypherQuery(cypher, {{"startName", startPage}, {"endName", endPage}});

        return ParseGenericNodeResult(data);
    } catch (const std::exception &e) {
        globalLogger->error("FindShortestPath failed: {}", e.what());
        return {};
    }
}

std::vector<NodeData> Neo4jInterface::GetLinkingPages(const std::string &pageName) {
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

std::vector<NodeData> Neo4jInterface::GetRandomConnectedPage(const std::vector<std::string> &existingNames) {
    if (!m_connected || existingNames.empty())
        return {};

    const std::string cypher = "UNWIND $names AS name "
                               "MATCH (a:PAGE {pageName: name})-[]-(neighbor:PAGE) "
                               "WHERE NOT neighbor.pageName IN $names "
                               "WITH neighbor, rand() AS r "
                               "ORDER BY r "
                               "RETURN neighbor "
                               "LIMIT 1";

    try {
        json data = ExecuteCypherQuery(cypher, {{"names", existingNames}});
        return ParsePagesFromResult(data);
    } catch (const std::exception &e) {
        globalLogger->error("GetRandomConnectedPage failed: {}", e.what());
        return {};
    }
}

std::vector<NodeData> Neo4jInterface::GetRandomPages(uint32_t count) {
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

std::vector<NodeData> Neo4jInterface::SearchPages(const std::string &queryString) {
    std::vector<NodeData> pages;

    if (!m_connected || queryString.empty()) {
        return pages;
    }

    try {
        const std::string cypher = "MATCH (p:PAGE) "
                                   "WHERE p.pageName STARTS WITH toLower($query) "
                                   "RETURN p.pageName AS pageName, p.title AS title "
                                   "LIMIT 25";

        json data = ExecuteCypherQuery(cypher, {{"query", queryString}});

        if (data.contains("results") && data["results"].is_array()) {
            for (const auto &result : data["results"]) {
                if (!result.contains("data") || !result["data"].is_array())
                    continue;

                for (const auto &entry : result["data"]) {
                    if (!entry.contains("row") || !entry["row"].is_array())
                        continue;

                    const auto &row = entry["row"];
                    if (row.size() >= 2) {
                        pages.emplace_back(NodeData{
                            row[0].get<std::string>(), // pageName
                            row[1].get<std::string>()  // title
                        });
                    }
                }
            }
        }

    } catch (const std::exception &e) {
        throw std::runtime_error("SearchPages failed: " + std::string(e.what()));
    }

    return pages;
}

// ------------------ HTTP Interface ------------------

std::vector<NodeData> HttpParsePagesFromResult(const json &data) {
    std::vector<NodeData> pages;

    if (!data.is_array())
        return pages;

    try {
        for (const auto &node : data) {
            pages.emplace_back(NodeData{node["pageName"].get<std::string>(), node["title"].get<std::string>()});
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

HttpInterface::~HttpInterface() {};

json HttpInterface::GetHttpResults(const std::string &endpoint, uint32_t timeoutMs = 0) {
    if (timeoutMs > 0) {
        setClientTimeoutMs(m_httpClient, timeoutMs);
    }

    auto res = m_httpClient->Get(endpoint);

    if (timeoutMs > 0) {
        setClientTimeoutMs(m_httpClient, m_timeout_ms);
    }

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

std::vector<NodeData> HttpInterface::GetLinkedPages(const std::string &pageName) {
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

std::vector<NodeData> HttpInterface::GetLinkingPages(const std::string &pageName) {
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

std::vector<NodeData> HttpInterface::FindShortestPath(const std::string &startPage, const std::string &endPage) {
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

std::vector<NodeData> HttpInterface::GetRandomConnectedPage(const std::vector<std::string> &existingNames) {
    if (!m_connected || existingNames.empty())
        return {};

    try {
        json body = {{"names", existingNames}};
        std::string bodyStr = body.dump();

        auto res = m_httpClient->Post("/random-connected-page", bodyStr, "application/json");
        if (!res)
            throw std::runtime_error("No response from server");
        if (res->status != httplib::StatusCode::OK_200)
            throw std::runtime_error("HTTP error " + std::to_string(res->status));

        json data = json::parse(res->body);
        return HttpParsePagesFromResult(data);
    } catch (const std::exception &e) {
        globalLogger->error("GetRandomConnectedPage failed: {}", e.what());
        return {};
    }
}

std::vector<NodeData> HttpInterface::GetRandomPages(uint32_t count) {
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
        json result = GetHttpResults("/connected", 100);

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

std::vector<NodeData> HttpInterface::SearchPages(const std::string &queryString) {
    std::vector<NodeData> pages;

    if (!m_connected || queryString.empty()) {
        return pages;
    }

    try {
        std::string encodedQuery;
        for (char c : queryString) {
            if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
                encodedQuery += c;
            } else {
                char buf[4];
                snprintf(buf, sizeof(buf), "%%%02X", static_cast<unsigned char>(c));
                encodedQuery += buf;
            }
        }

        const std::string endpoint = "/search-pages?query=" + encodedQuery;

        json data = GetHttpResults(endpoint);

        pages = HttpParsePagesFromResult(data);

    } catch (const std::exception &e) {
        throw std::runtime_error("SearchPages failed for query '" + queryString + "': " + std::string(e.what()));
    }

    return pages;
}
