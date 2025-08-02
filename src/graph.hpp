#ifndef GRAPH_H
#define GRAPH_H

#include <atomic>
#include <cstdint>
#include <cstring>
#include <glm/glm.hpp>
#include <random>
#include <string>
#include <vector>

namespace GS {

struct Color {
    unsigned char r, g, b;
};

// Structure of Arrays for better cache performance and GPU compatibility
struct NodeData {
    std::vector<std::string> titles;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
    std::vector<glm::vec3> forces;
    std::vector<Color> colors;
    std::vector<unsigned char> sizes;
    std::vector<unsigned char> edgeSizes;
    std::vector<bool> fixed;
    std::vector<float> masses;

    NodeData() = default;
    NodeData(const NodeData &) = default;
    NodeData &operator=(const NodeData &) = default;

    NodeData(NodeData &&x) noexcept
        : titles(std::move(x.titles)), positions(std::move(x.positions)), velocities(std::move(x.velocities)),
          forces(std::move(x.forces)), colors(std::move(x.colors)), sizes(std::move(x.sizes)),
          edgeSizes(std::move(x.edgeSizes)), fixed(std::move(x.fixed)), masses(std::move(x.masses)) {}

    NodeData &operator=(NodeData &&x) noexcept {
        if (this != &x) {
            titles = std::move(x.titles);
            positions = std::move(x.positions);
            velocities = std::move(x.velocities);
            forces = std::move(x.forces);
            colors = std::move(x.colors);
            sizes = std::move(x.sizes);
            edgeSizes = std::move(x.edgeSizes);
            fixed = std::move(x.fixed);
            masses = std::move(x.masses);
        }
        return *this;
    }
};

struct EdgeData {
    std::vector<uint32_t> startIdxs;
    std::vector<uint32_t> endIdxs;

    EdgeData() = default;
    EdgeData(const EdgeData &) = default;
    EdgeData &operator=(const EdgeData &) = default;

    EdgeData(EdgeData &&other) noexcept : startIdxs(std::move(other.startIdxs)), endIdxs(std::move(other.endIdxs)) {}

    EdgeData &operator=(EdgeData &&other) noexcept {
        if (this != &other) {
            startIdxs = std::move(other.startIdxs);
            endIdxs = std::move(other.endIdxs);
        }
        return *this;
    }
};

// A graph storage class using Structure of Arrays (SoA) for better performance
class Graph {
  public:
    Graph() = default;
    ~Graph() = default;
    Graph(const Graph &other) : nodes(other.nodes), edges(other.edges) {}
    Graph(Graph &&other) noexcept : nodes(std::move(other.nodes)), edges(std::move(other.edges)) {}
    Graph &operator=(const Graph &other);
    Graph &operator=(Graph &&other) noexcept;

    // Node management
    uint32_t AddNode(const std::string &title);
    void ReserveNodes(const uint32_t N);
    void ReserveEdges(const uint32_t N);
    void ResizeNodes(const uint32_t N);
    void ResizeEdges(const uint32_t N);
    void GenerateDefaultData(); // Generate positions, velocities, etc.
    void Clear();

    // Edge management
    void AddEdge(uint32_t idx1, uint32_t idx2);

    // Query functions
    std::vector<uint32_t> GetNeighboursIdx(uint32_t rootIdx) const;
    uint32_t GetTopNode();

    // File I/O (only saves/loads titles and edges)
    bool SaveBinary(const std::string &filename) const;
    bool LoadBinary(const std::string &filename);

    // Utility functions

    float GetRadius() const;
    glm::vec3 GetCenter() const;
    size_t GetNodeCount() const { return nodes.titles.size(); }
    size_t GetEdgeCount() const { return edges.startIdxs.size(); }

    // Data access
    NodeData nodes;
    EdgeData edges;

    // Helper functions for accessing specific node data
    const std::string &GetNodeTitle(size_t idx) const { return nodes.titles[idx]; }

    const glm::vec3 &GetNodePosition(size_t idx) const { return nodes.positions[idx]; }

    void SetNodePosition(size_t idx, const glm::vec3 &pos) {
        if (idx < nodes.positions.size()) {
            nodes.positions[idx] = pos;
        }
    }

    bool IsNodeFixed(size_t idx) const { return idx < nodes.fixed.size() ? nodes.fixed[idx] : false; }

    void SetNodeFixed(size_t idx, bool fixed) {
        if (idx < nodes.fixed.size()) {
            nodes.fixed[idx] = fixed;
        }
    }

  private:
    static glm::vec3 getRandVec3(float min = -100.0f, float max = 100.0f) {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<float> dis(min, max);
        return glm::vec3(dis(gen), dis(gen), dis(gen));
    }

    static float getRandFloat(float min = -100.0f, float max = 100.0f) {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }
};

// Triple buffer system for thread-safe graph updates
class GraphTripleBuf {
  public:
    GraphTripleBuf();
    ~GraphTripleBuf();

    Graph *GetCurrent();
    Graph *GetWriteBuffer() { return m_buffers[m_write]; }
    void Publish();
    void PublishAll();
    uint32_t Version();

  private:
    static const uint32_t BUFFERCOUNT = 3;

    Graph *m_buffers[BUFFERCOUNT];
    std::atomic<uint32_t> m_version{0};
    std::atomic<uint32_t> m_current{0};
    uint32_t m_write = 1;
    uint32_t m_spare = 2;
};

} // namespace GS

#endif
