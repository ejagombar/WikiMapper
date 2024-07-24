#include <httplib.h>
#include <string>

struct Neo4jData {
    int port;
    std::string host;
    std::string auth64;
    std::string dbName;
};

class Neo4j {
  public:
    Neo4j(const Neo4jData in);
    httplib::Result shortestPath(std::string &start, std::string &end, int timeout);

  private:
    httplib::Headers m_headers;
    httplib::Client cli;
    std::string m_path;
};
