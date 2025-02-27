#ifndef GRAPH_H
#define GRAPH_H

#include <cstdint>
#include <cstring>
#include <glm/glm.hpp>
#include <vector>

namespace GraphDB {

struct Node {
    char title[64];
    float colour;
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 force;

    Node(const char *t) {
        strncpy(title, t, sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0'; // Ensure null-termination
    }
};

struct Edge {
    uint32_t startIdx;
    uint32_t endIdx;
};

class Graph {
  public:
    uint32_t AddNode(const char *title);
    void AddEdge(uint32_t idx1, uint32_t idx2);

    std::vector<uint32_t> GetNeighboursIdx(uint32_t rootIdx) const;
    std::vector<Node> GetNeighbours(uint32_t rootIdx) const;

    uint32_t GetTopNode();

    std::vector<Node> m_nodes;
    std::vector<Edge> m_edges;
};

} // namespace GraphDB

#endif
