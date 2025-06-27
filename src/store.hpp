#ifndef STORE_H
#define STORE_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "../lib/json.hpp"
#include <httplib.h>

struct NodeStore {
    uint32_t UID;
    std::string name;
    std::vector<uint32_t> linksTo;
};

struct LinkedPage {
    std::string pageName;
    std::string title;
};

using json = nlohmann::json;

std::vector<LinkedPage> ParsePagesFromResult(const json &data);

class dBInterface {
  public:
    dBInterface() = default;
    ~dBInterface() = default;

    virtual std::vector<LinkedPage> GetLinkedPages(const std::string &pageName) = 0;
    virtual std::vector<LinkedPage> GetLinkingPages(const std::string &pageName) = 0;
    virtual std::vector<LinkedPage> FindShortestPath(const std::string &startPage, const std::string &endPage) = 0;
    virtual std::vector<LinkedPage> GetRandomPages(uint32_t count) = 0;
};

class Neo4jInterface : private dBInterface {
  public:
    Neo4jInterface(const std::string url);
    ~Neo4jInterface() = default;

    bool Authenticate(const std::string username, const std::string password);

    std::vector<LinkedPage> GetLinkedPages(const std::string &pageName);
    std::vector<LinkedPage> GetLinkingPages(const std::string &pageName);
    std::vector<LinkedPage> FindShortestPath(const std::string &startPage, const std::string &endPage);
    std::vector<LinkedPage> GetRandomPages(uint32_t count);

  private:
    json ExecuteCypherQuery(const std::string &cypher, const json &parameters);

    std::string m_url;
    std::unique_ptr<httplib::Client> m_httpClient;
};

class HttpInterface : private dBInterface {
  public:
    HttpInterface(const std::string domain);
    ~HttpInterface() = default;

    std::vector<LinkedPage> GetLinkedPages(const std::string &pageName);
    std::vector<LinkedPage> GetLinkingPages(const std::string &pageName);
    std::vector<LinkedPage> FindShortestPath(const std::string &startPage, const std::string &endPage);
    std::vector<LinkedPage> GetRandomPages(uint32_t count);

  private:
    json GetHttpResults(const std::string &endpoint);
    std::unique_ptr<httplib::Client> m_httpClient;
};

#endif
