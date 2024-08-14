#ifndef GRAPH_H
#define GRAPH_H

#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <vector>

namespace GraphDB {

struct Node {
    const uint32_t UID;
    char title[256];

    Node(uint32_t id, const char *t) : UID(id) {
        strncpy(title, t, sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0'; // Ensure null-termination
    }
};

class Graph {
  private:
    std::unordered_map<uint32_t, Node> nodes;

    std::unordered_map<uint32_t, std::vector<uint32_t>> adjList;

  public:
    void addNode(uint32_t uid, const char *title);

    void addEdge(uint32_t uid1, uint32_t uid2);

    const Node *getNode(uint32_t uid) const;

    std::vector<Node> getAllNodes() const;

    std::vector<std::pair<uint32_t, uint32_t>> getAllLinks() const;

    std::vector<uint32_t> getNeighbors(uint32_t uid) const;

    void printGraph() const;
};

} // namespace GraphDB
#endif

void generateFakeData(GraphDB::Graph &graph);
