#include "graph.hpp"
#include <atomic>
#include <cstdint>
#include <iostream>
#include <vector>

namespace GS {

// ------------------ Graph ------------------

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
    for (uint32_t i = 0; i < nodes.size(); i++) {
        int linkCount = GetNeighboursIdx(i).size();
        if (linkCount > maxLinkCount) {
            maxLinkCount = linkCount;
            topNode = i;
        }
    }
    return topNode;
}

Graph &Graph::operator=(Graph &other) {
    nodes = other.nodes;
    edges = other.edges;

    return other;
}

void Graph::Clear() {
    nodes.clear();
    edges.clear();
};

// ------------------ GraphTripleBuffer ------------------

GraphTripleBuf::GraphTripleBuf() {
    for (uint32_t i = 0; i < BUFFERCOUNT; i++) {
        m_buffers[i] = new Graph();
    }
};

GraphTripleBuf::~GraphTripleBuf() {
    for (uint32_t i = 0; i < BUFFERCOUNT; i++) {
        delete m_buffers[i];
    }
};

// This method must be called to update the write buffer after it has been written to.
void GraphTripleBuf::Publish() {
    const uint32_t oldCurrent = m_current.load(std::memory_order_relaxed);
    m_current.store(m_write, std::memory_order_release);
    const uint32_t tmp = m_spare;
    m_spare = oldCurrent;
    m_write = tmp;
    m_version.fetch_add(1, std::memory_order_release);
};

void GraphTripleBuf::PublishAll() {
    Publish();

    GS::Graph *readgraph = GetCurrent();
    GS::Graph *writeGraph = GetWriteBuffer();

    *writeGraph = *readgraph;
    Publish();

    readgraph = GetCurrent();
    writeGraph = GetWriteBuffer();

    *writeGraph = *readgraph;
    Publish();
}

// Get the current buffer to read from.
Graph *GraphTripleBuf::GetCurrent() {
    const uint32_t idx = m_current.load(std::memory_order_acquire);
    return m_buffers[idx];
};

// Get the current iteration of the graph. This can be used to check if the graph has been updated since it was last
// checked without needing to read the graph data.
uint32_t GraphTripleBuf::Version() { return m_version.load(std::memory_order_acquire); };

} // namespace GS
