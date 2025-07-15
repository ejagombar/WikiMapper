#include "graph.hpp"
#include "logger.hpp"
#include <atomic>
#include <cstdint>
#include <random>
#include <vector>

namespace GS {

// ------------------ Graph ------------------

uint32_t Graph::AddNode(std::string title) {
    nodes.titles.emplace_back(title);
    return nodes.titles.size() - 1;
}

void Graph::AddDefaultData() {
    const auto N = nodes.titles.size();

    nodes.velocities.resize(N, glm::vec3(0, 0, 0));
    nodes.forces.resize(N, glm::vec3(0, 0, 0));
    nodes.colors.resize(N, Color{200, 200, 200});
    nodes.sizes.resize(N, 10);
    nodes.edgeSizes.resize(N, 1);
    nodes.fixed.resize(false);
    nodes.masses.resize(N, 1.0f);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-100.0f, 100.0f);

    nodes.positions.resize(N);
    for (uint32_t i = 0; i < N; ++i) {
        nodes.positions[i] = glm::vec3(dis(gen), dis(gen), dis(gen));
    }
}

void Graph::AddEdge(uint32_t idx1, uint32_t idx2) {
    if (idx1 >= nodes.titles.size() || idx2 > nodes.titles.size()) {
        globalLogger->error("Error: Idx out of range.");
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
    for (int i = 0; i < edges.startIdxs.size(); i++) {
        if (edges.startIdxs[i] == idx1 && edges.endIdxs[i] == idx2) {
            return;
        }
    }
    edges.startIdxs.push_back(idx1);
    edges.endIdxs.push_back(idx2);
}

std::vector<uint32_t> Graph::GetNeighboursIdx(uint32_t idx) const {
    std::vector<uint32_t> out;

    for (int i = 0; i < edges.startIdxs.size(); i++) {
        if (edges.startIdxs[i] > idx)
            continue;

        if (edges.startIdxs[i] == idx) {
            out.push_back(edges.endIdxs[i]);
        } else if (edges.endIdxs[i] == idx) {
            out.push_back(edges.startIdxs[i]);
        }
    }

    return out;
}

uint32_t Graph::GetTopNode() {
    int maxLinkCount(0);
    uint32_t topNode(0);
    for (uint32_t i = 0; i < nodes.titles.size(); i++) {
        int linkCount = GetNeighboursIdx(i).size();
        if (linkCount > maxLinkCount) {
            maxLinkCount = linkCount;
            topNode = i;
        }
    }
    return topNode;
}

Graph &Graph::operator=(const Graph &other) {
    if (this != &other) {
        nodes = other.nodes;
        edges = other.edges;
    }
    return *this;
}

void Graph::Clear() {
    nodes.titles.clear();
    nodes.positions.clear();
    nodes.velocities.clear();
    nodes.forces.clear();
    nodes.colors.clear();
    nodes.sizes.clear();
    nodes.edgeSizes.clear();
    nodes.fixed.clear();
    nodes.masses.clear();

    edges.startIdxs.clear();
    edges.endIdxs.clear();
};

// Binary load function
bool Graph::LoadBinary(const std::string &filename) {
    // std::ifstream file(filename, std::ios::binary);
    // if (!file.is_open()) {
    //     std::cerr << "Error: Could not open file for reading: " << filename << std::endl;
    //     return false;
    // }
    //
    // try {
    //     // Clear existing data
    //     Clear();
    //
    //     // Read header
    //     uint32_t version, nodeCount, edgeCount;
    //     file.read(reinterpret_cast<char *>(&version), sizeof(version));
    //     file.read(reinterpret_cast<char *>(&nodeCount), sizeof(nodeCount));
    //     file.read(reinterpret_cast<char *>(&edgeCount), sizeof(edgeCount));
    //
    //     if (version != 1) {
    //         std::cerr << "Error: Unsupported file version: " << version << std::endl;
    //         return false;
    //     }
    //
    //     nodes.reserve(nodeCount);
    //     edges.reserve(edgeCount);
    //
    //     for (uint32_t i = 0; i < nodeCount; ++i) {
    //         Node node("");
    //         file.read(node.title, sizeof(node.title));
    //         file.read(reinterpret_cast<char *>(&node.pos), sizeof(node.pos));
    //         file.read(reinterpret_cast<char *>(&node.vel), sizeof(node.vel));
    //         file.read(reinterpret_cast<char *>(&node.force), sizeof(node.force));
    //         file.read(reinterpret_cast<char *>(node.rgb), sizeof(node.rgb));
    //         file.read(reinterpret_cast<char *>(&node.size), sizeof(node.size));
    //         file.read(reinterpret_cast<char *>(&node.edgeSize), sizeof(node.edgeSize));
    //         file.read(reinterpret_cast<char *>(&node.fixed), sizeof(node.fixed));
    //         file.read(reinterpret_cast<char *>(&node.mass), sizeof(node.mass));
    //
    //         node.title[sizeof(node.title) - 1] = '\0';
    //         nodes.push_back(node);
    //     }
    //
    //     for (uint32_t i = 0; i < edgeCount; ++i) {
    //         Edge edge;
    //         file.read(reinterpret_cast<char *>(&edge.startIdx), sizeof(edge.startIdx));
    //         file.read(reinterpret_cast<char *>(&edge.endIdx), sizeof(edge.endIdx));
    //         edges.push_back(edge);
    //     }
    //
    //     file.close();
    //     return true;
    //
    // } catch (const std::exception &e) {
    //     std::cerr << "Error reading binary file: " << e.what() << std::endl;
    //     Clear();
    //     return false;
    // }
    return false;
}

bool Graph::SaveBinary(const std::string &filename) const {
    // std::ofstream file(filename, std::ios::binary);
    // if (!file.is_open()) {
    //     std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
    //     return false;
    // }
    //
    // try {
    //     const uint32_t version = 1;
    //     const uint32_t nodeCount = static_cast<uint32_t>(nodes.size());
    //     const uint32_t edgeCount = static_cast<uint32_t>(edges.size());
    //
    //     file.write(reinterpret_cast<const char *>(&version), sizeof(version));
    //     file.write(reinterpret_cast<const char *>(&nodeCount), sizeof(nodeCount));
    //     file.write(reinterpret_cast<const char *>(&edgeCount), sizeof(edgeCount));
    //
    //     for (const auto &node : nodes) {
    //         file.write(node.title, sizeof(node.title));
    //         file.write(reinterpret_cast<const char *>(&node.pos), sizeof(node.pos));
    //         file.write(reinterpret_cast<const char *>(&node.vel), sizeof(node.vel));
    //         file.write(reinterpret_cast<const char *>(&node.force), sizeof(node.force));
    //         file.write(reinterpret_cast<const char *>(node.rgb), sizeof(node.rgb));
    //         file.write(reinterpret_cast<const char *>(&node.size), sizeof(node.size));
    //         file.write(reinterpret_cast<const char *>(&node.edgeSize), sizeof(node.edgeSize));
    //         file.write(reinterpret_cast<const char *>(&node.fixed), sizeof(node.fixed));
    //         file.write(reinterpret_cast<const char *>(&node.mass), sizeof(node.mass));
    //     }
    //
    //     for (const auto &edge : edges) {
    //         file.write(reinterpret_cast<const char *>(&edge.startIdx), sizeof(edge.startIdx));
    //         file.write(reinterpret_cast<const char *>(&edge.endIdx), sizeof(edge.endIdx));
    //     }
    //
    //     file.close();
    //     return true;
    // } catch (const std::exception &e) {
    //     std::cerr << "Error writing binary file: " << e.what() << std::endl;
    //     return false;
    // }

    return false;
}

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
