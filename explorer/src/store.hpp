#ifndef STORE_H
#define STORE_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

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

class Neo4jInterface {
  public:
    Neo4jInterface(const std::string url);
    ~Neo4jInterface() {};

    bool Authenticate(const std::string username, const std::string password);
    std::vector<LinkedPage> GetLinkedPages(const std::string pageName);

  private:
    std::string m_url;
    std::unique_ptr<httplib::Client> m_httpClient;
};

typedef std::vector<NodeStore> DB;

#endif
