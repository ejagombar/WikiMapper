#ifndef INTERFACE_H
#define INTERFACE_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "../lib/json.hpp"
#include <httplib.h>

struct LinkedPage {
    std::string pageName;
    std::string title;
};

using json = nlohmann::json;

std::vector<LinkedPage> ParsePagesFromResult(const json &data);

class dBInterface {
  public:
    dBInterface() = default;
    virtual ~dBInterface() = default;

    virtual bool RequiresAuthentication() const = 0;
    virtual bool Authenticate([[maybe_unused]] const std::string &username,
                              [[maybe_unused]] const std::string &password) {
        return true;
    }

    virtual bool connected() = 0;
    virtual std::vector<LinkedPage> GetLinkedPages(const std::string &queryString) = 0;
    virtual std::vector<LinkedPage> GetLinkingPages(const std::string &queryString) = 0;
    virtual std::vector<LinkedPage> FindShortestPath(const std::string &startPage, const std::string &endPage) = 0;
    virtual std::vector<LinkedPage> GetRandomPages(uint32_t count) = 0;
    virtual std::vector<LinkedPage> SearchPages(const std::string &queryString) = 0;
};

class Neo4jInterface : public dBInterface {
  public:
    Neo4jInterface(const std::string url);
    ~Neo4jInterface();

    bool RequiresAuthentication() const override { return true; }
    bool Authenticate(const std::string &username, const std::string &password) override;

    virtual bool connected() override;
    std::vector<LinkedPage> GetLinkedPages(const std::string &queryString) override;
    std::vector<LinkedPage> GetLinkingPages(const std::string &queryString) override;
    std::vector<LinkedPage> FindShortestPath(const std::string &startPage, const std::string &endPage) override;
    std::vector<LinkedPage> GetRandomPages(uint32_t count) override;
    std::vector<LinkedPage> SearchPages(const std::string &queryString) override;

  private:
    json ExecuteCypherQuery(const std::string &cypher, const json &parameters);

    std::string m_url;
    bool m_connected = false;
    uint32_t m_timeout_ms{5000};
    std::unique_ptr<httplib::Client> m_httpClient;
};

class HttpInterface : public dBInterface {
  public:
    HttpInterface(const std::string domain);
    ~HttpInterface();

    bool RequiresAuthentication() const override { return false; }

    virtual bool connected() override;
    std::vector<LinkedPage> GetLinkedPages(const std::string &pageName) override;
    std::vector<LinkedPage> GetLinkingPages(const std::string &pageName) override;
    std::vector<LinkedPage> FindShortestPath(const std::string &startPage, const std::string &endPage) override;
    std::vector<LinkedPage> GetRandomPages(uint32_t count) override;
    std::vector<LinkedPage> SearchPages(const std::string &queryString) override;

  private:
    json GetHttpResults(const std::string &endpoint, uint32_t timeoutMs);
    std::unique_ptr<httplib::Client> m_httpClient;
    uint32_t m_timeout_ms{5000};
    bool m_connected = false;
};

#endif
