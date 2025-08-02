#include "graph.hpp"
#include "../lib/rgb_hsv.hpp"
#include "logger.hpp"
#include <atomic>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

namespace GS {

// ------------------ Graph ------------------

uint32_t Graph::AddNode(const std::string &title) {
    nodes.titles.emplace_back(title);

    nodes.positions.emplace_back(getRandVec3());
    nodes.velocities.emplace_back(glm::vec3(0, 0, 0));
    nodes.forces.emplace_back(glm::vec3(0, 0, 0));
    nodes.sizes.push_back(20);
    nodes.edgeSizes.push_back(10);
    nodes.fixed.push_back(false);
    nodes.masses.push_back(1.0f);

    auto col = hsv2rgb(getRandFloat(0, 1), 0.8f, 1.0f);
    nodes.colors.emplace_back(GS::Color{static_cast<unsigned char>(col.r), static_cast<unsigned char>(col.g),
                                        static_cast<unsigned char>(col.b)});

    return nodes.titles.size() - 1;
}

void Graph::ReserveNodes(const uint32_t N) {
    nodes.titles.reserve(N);
    nodes.positions.reserve(N);
    nodes.velocities.reserve(N);
    nodes.forces.reserve(N);
    nodes.colors.reserve(N);
    nodes.sizes.reserve(N);
    nodes.edgeSizes.reserve(N);
    nodes.fixed.reserve(N);
    nodes.masses.reserve(N);
}

void Graph::ReserveEdges(const uint32_t N) {
    edges.startIdxs.reserve(N);
    edges.endIdxs.reserve(N);
}

void Graph::ResizeNodes(const uint32_t N) {
    nodes.titles.resize(N);
    nodes.positions.resize(N);
    nodes.velocities.resize(N, glm::vec3(0, 0, 0));
    nodes.forces.resize(N, glm::vec3(0, 0, 0));
    nodes.colors.resize(N, Color{200, 200, 200});
    nodes.sizes.resize(N, 20);
    nodes.edgeSizes.resize(N, 10);
    nodes.fixed.resize(N, false);
    nodes.masses.resize(N, 1.0f);
}

void Graph::ResizeEdges(const uint32_t N) {
    edges.startIdxs.resize(N);
    edges.endIdxs.resize(N);
}

void Graph::GenerateDefaultData() {
    const auto N = nodes.titles.size();

    if (N == 0) {
        globalLogger->warn("GenerateDefaultData called with 0 nodes");
        return;
    }

    globalLogger->info("Generating default data for {} nodes", N);

    nodes.positions.resize(N);
    nodes.velocities.resize(N);
    nodes.forces.resize(N);
    nodes.colors.resize(N);
    nodes.sizes.resize(N);
    nodes.edgeSizes.resize(N);
    nodes.fixed.resize(N);
    nodes.masses.resize(N);

    for (size_t i = 0; i < N; ++i) {
        nodes.positions[i] = getRandVec3();
        nodes.velocities[i] = glm::vec3(0, 0, 0);
        nodes.forces[i] = glm::vec3(0, 0, 0);
        nodes.colors[i] = Color{200, 200, 200};
        nodes.sizes[i] = 60;
        nodes.edgeSizes[i] = 10;
        nodes.fixed[i] = false;
        nodes.masses[i] = 1.0f;
    }
}

void Graph::AddEdge(uint32_t idx1, uint32_t idx2) {
    const uint32_t nodeCount = static_cast<uint32_t>(nodes.titles.size());

    if (idx1 >= nodeCount || idx2 >= nodeCount) {
        globalLogger->error("Error: Edge index out of range. idx1={}, idx2={}, nodeCount={}", idx1, idx2, nodeCount);
        return;
    }

    // Do not add self links
    if (idx1 == idx2) {
        return;
    }

    // Ensure the first idx is always lowest (will simplify things later on)
    if (idx1 > idx2) {
        std::swap(idx1, idx2);
    }

    // Ensure the edge is not a duplicate
    const size_t edgeCount = edges.startIdxs.size();
    for (size_t i = 0; i < edgeCount; i++) {
        if (edges.startIdxs[i] == idx1 && edges.endIdxs[i] == idx2) {
            return;
        }
    }

    edges.startIdxs.push_back(idx1);
    edges.endIdxs.push_back(idx2);
}

std::vector<uint32_t> Graph::GetNeighboursIdx(uint32_t idx) const {
    std::vector<uint32_t> out;

    const size_t edgeCount = edges.startIdxs.size();
    for (size_t i = 0; i < edgeCount; i++) {
        if (edges.startIdxs[i] == idx) {
            out.push_back(edges.endIdxs[i]);
        } else if (edges.endIdxs[i] == idx) {
            out.push_back(edges.startIdxs[i]);
        }
    }

    return out;
}

uint32_t Graph::GetTopNode() {
    int maxLinkCount = 0;
    uint32_t topNode = 0;
    const uint32_t nodeCount = static_cast<uint32_t>(nodes.titles.size());

    for (uint32_t i = 0; i < nodeCount; i++) {
        int linkCount = static_cast<int>(GetNeighboursIdx(i).size());
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

Graph &Graph::operator=(Graph &&other) noexcept {
    if (this != &other) {
        nodes = std::move(other.nodes);
        edges = std::move(other.edges);
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
}

// Binary load function - only loads titles and edges, generates other data
bool Graph::LoadBinary(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        globalLogger->error("Error: Could not open file for reading: {}", filename);
        return false;
    }

    try {
        Clear();

        uint32_t version, nodeCount, edgeCount;
        file.read(reinterpret_cast<char *>(&version), sizeof(version));
        file.read(reinterpret_cast<char *>(&nodeCount), sizeof(nodeCount));
        file.read(reinterpret_cast<char *>(&edgeCount), sizeof(edgeCount));

        if (version != 2) {
            globalLogger->error("Error: Unsupported file version: {}", version);
            return false;
        }

        globalLogger->info("Loading graph: {} nodes, {} edges", nodeCount, edgeCount);

        // Reserve space for efficiency
        ReserveNodes(nodeCount);
        ReserveEdges(edgeCount);

        // Read node titles
        for (uint32_t i = 0; i < nodeCount; ++i) {
            uint32_t titleLen;
            file.read(reinterpret_cast<char *>(&titleLen), sizeof(titleLen));

            if (titleLen > 10000) { // Sanity check
                globalLogger->error("Error: Title length too large: {}", titleLen);
                return false;
            }

            std::string title(titleLen, '\0');
            file.read(&title[0], titleLen);
            nodes.titles.push_back(title);
        }

        // Read edges
        edges.startIdxs.resize(edgeCount);
        edges.endIdxs.resize(edgeCount);

        for (uint32_t i = 0; i < edgeCount; ++i) {
            file.read(reinterpret_cast<char *>(&edges.startIdxs[i]), sizeof(edges.startIdxs[i]));
            file.read(reinterpret_cast<char *>(&edges.endIdxs[i]), sizeof(edges.endIdxs[i]));

            // Validate edge indices
            if (edges.startIdxs[i] >= nodeCount || edges.endIdxs[i] >= nodeCount) {
                globalLogger->error("Error: Invalid edge indices: {} -> {}", edges.startIdxs[i], edges.endIdxs[i]);
                return false;
            }
        }

        file.close();

        // Generate all other node data (positions, velocities, etc.)
        GenerateDefaultData();

        globalLogger->info("Graph loaded successfully: {} nodes, {} edges", nodes.titles.size(),
                           edges.startIdxs.size());
        return true;

    } catch (const std::exception &e) {
        globalLogger->error("Error reading binary file: {}", e.what());
        Clear();
        return false;
    }
}

// Binary save function - only saves titles and edges
bool Graph::SaveBinary(const std::string &filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        globalLogger->error("Error: Could not open file for writing: {}", filename);
        return false;
    }

    try {
        const uint32_t version = 2;
        const uint32_t nodeCount = static_cast<uint32_t>(nodes.titles.size());
        const uint32_t edgeCount = static_cast<uint32_t>(edges.startIdxs.size());

        globalLogger->info("Saving graph: {} nodes, {} edges", nodeCount, edgeCount);

        // Write header
        file.write(reinterpret_cast<const char *>(&version), sizeof(version));
        file.write(reinterpret_cast<const char *>(&nodeCount), sizeof(nodeCount));
        file.write(reinterpret_cast<const char *>(&edgeCount), sizeof(edgeCount));

        // Write node titles
        for (uint32_t i = 0; i < nodeCount; ++i) {
            uint32_t titleLen = static_cast<uint32_t>(nodes.titles[i].size());
            file.write(reinterpret_cast<const char *>(&titleLen), sizeof(titleLen));
            file.write(nodes.titles[i].data(), titleLen);
        }

        // Write edges
        for (uint32_t i = 0; i < edgeCount; ++i) {
            file.write(reinterpret_cast<const char *>(&edges.startIdxs[i]), sizeof(edges.startIdxs[i]));
            file.write(reinterpret_cast<const char *>(&edges.endIdxs[i]), sizeof(edges.endIdxs[i]));
        }

        file.close();
        globalLogger->info("Graph saved successfully to: {}", filename);
        return true;

    } catch (const std::exception &e) {
        globalLogger->error("Error writing binary file: {}", e.what());
        return false;
    }
}

// ------------------ GraphTripleBuffer ------------------

GraphTripleBuf::GraphTripleBuf() {
    for (uint32_t i = 0; i < BUFFERCOUNT; i++) {
        m_buffers[i] = new Graph();
    }
}

GraphTripleBuf::~GraphTripleBuf() {
    for (uint32_t i = 0; i < BUFFERCOUNT; i++) {
        delete m_buffers[i];
    }
}

void GraphTripleBuf::Publish() {
    const uint32_t oldCurrent = m_current.load(std::memory_order_relaxed);
    m_current.store(m_write, std::memory_order_release);
    const uint32_t tmp = m_spare;
    m_spare = oldCurrent;
    m_write = tmp;
    m_version.fetch_add(1, std::memory_order_release);
}

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

Graph *GraphTripleBuf::GetCurrent() {
    const uint32_t idx = m_current.load(std::memory_order_acquire);
    return m_buffers[idx];
}

uint32_t GraphTripleBuf::Version() { return m_version.load(std::memory_order_acquire); }

} // namespace GS
