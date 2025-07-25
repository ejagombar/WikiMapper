#ifndef GRAPH_H
#define GRAPH_H

#include <atomic>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <glm/glm.hpp>
#include <iomanip>
#include <random>
#include <sstream>
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
};

struct EdgeData {
    std::vector<uint32_t> startIdxs;
    std::vector<uint32_t> endIdxs;
};

// A graph storage class using Structure of Arrays (SoA) for better performance
class Graph {
  public:
    Graph() = default;
    ~Graph() = default;

    Graph &operator=(const Graph &other);

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
