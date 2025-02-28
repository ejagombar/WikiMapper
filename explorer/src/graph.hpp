#ifndef GRAPH_H
#define GRAPH_H

#include <cstdint>
#include <cstring>
#include <glm/glm.hpp>
#include <vector>

namespace GS {

struct Node {
    char title[64];
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 force;
    unsigned char rgb[3];
    unsigned char size;
    unsigned char edgeSize;

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

    Node &EdgeStart(uint32_t idx) { return nodes.at(edges.at(idx).startIdx); }
    Node &EdgeEnd(uint32_t idx) { return nodes.at(edges.at(idx).endIdx); }

    std::vector<Node> nodes; // It feels weird to have the m_ here as they are public. Idk what is best.
    std::vector<Edge> edges;
};

} // namespace GS
#endif
