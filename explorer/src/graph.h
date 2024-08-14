#ifndef GRAPH_H
#define GRAPH_H

#include <cstdint>
#include <cstring>
#include <list>
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

    std::unordered_map<uint32_t, std::list<uint32_t>> adjList;

  public:
    void addNode(uint32_t uid, const char *title);

    void addEdge(uint32_t uid1, uint32_t uid2);

    const Node *getNode(uint32_t uid) const;

    std::vector<Node> getAllNodes() const;

    std::list<uint32_t> getNeighbors(uint32_t uid) const;

    void printGraph() const;
};

void generateFakeData(GraphDB::Graph &graph);

} // namespace GraphDB
#endif
