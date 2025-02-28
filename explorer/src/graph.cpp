#include "graph.hpp"
#include <cstdint>
#include <iostream>
#include <vector>

namespace GS {

uint32_t Graph::AddNode(const char *title) {
    nodes.emplace_back(Node(title));
    return nodes.size() - 1;
}

void Graph::AddEdge(uint32_t idx1, uint32_t idx2) {
    if (idx1 >= nodes.size() || idx2 > nodes.size()) {
        std::cerr << "Error: Idx out of range." << std::endl;
    }

    // Do not add self links
    if (idx1 == idx2) {
        return;
    }

    // Ensure the first idx is always lowest (will simplify things later on)
    if (idx1 > idx2) {
        const uint32_t tmp(idx1);
        idx1 = idx2;
        idx2 = tmp;
    }

    // Ensure the edge is not a duplicate
    for (const Edge &e : edges) {
        if (e.startIdx == idx1 && e.endIdx == idx2) {
            return;
        }
    }

    edges.emplace_back(Edge(idx1, idx2));
}

std::vector<Node> Graph::GetNeighbours(uint32_t idx) const {
    std::vector<Node> out;

    for (const Edge &e : edges) {
        if (e.startIdx > idx)
            continue;

        if (e.startIdx == idx) {
            out.emplace_back(nodes[e.endIdx]);
        } else if (e.endIdx == idx) {
            out.emplace_back(nodes[e.startIdx]);
        }
    }

    return out;
}

std::vector<uint32_t> Graph::GetNeighboursIdx(uint32_t idx) const {
    std::vector<uint32_t> out;

    for (const Edge &e : edges) {
        if (e.startIdx > idx)
            continue;

        if (e.startIdx == idx) {
            out.push_back(e.endIdx);
        } else if (e.endIdx == idx) {
            out.push_back(e.startIdx);
        }
    }

    return out;
}

uint32_t Graph::GetTopNode() {
    int maxLinkCount(0);
    uint32_t topNode(0);
    for (uint i = 0; i < nodes.size(); i++) {
        int linkCount = GetNeighboursIdx(i).size();
        if (linkCount > maxLinkCount) {
            maxLinkCount = linkCount;
            topNode = i;
        }
    }
    return topNode;
}

} // namespace GS
