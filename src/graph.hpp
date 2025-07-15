#ifndef GRAPH_H
#define GRAPH_H

#include <atomic>
#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace GS {
// TODO: USE STRUCTURE OF ARRAYS FOR SIMD TO IMPROVE PEFROMANCE

// The graph datastructure nodes are heavily linked to nodes that are displayed in the viewer. This allowed an
// intemediary layer to be removed from the engine, decreasing the number of copies, at the cost of modularity.
// struct Node {
//     char title[64];
//     glm::vec3 pos = glm::vec3(0, 0, 0);
//     glm::vec3 vel = glm::vec3(0, 0, 0);
//     glm::vec3 force = glm::vec3(0, 0, 0);
//     unsigned char rgb[3];
//     unsigned char size = 10;
//     unsigned char edgeSize;
//     bool fixed = false; // Whether this node's position is fixed (e.g., when dragged by user)
//     float mass = 1.0f;  // Node mass, affects force response
//
//     Node(const char *t) {
//         strncpy(title, t, sizeof(title) - 1);
//         title[sizeof(title) - 1] = '\0'; // Ensure null-termination
//
//         std::random_device rd;
//         std::mt19937 gen(rd());
//         std::uniform_real_distribution<> dis(-100.0, 100.0);
//         pos = glm::vec3(dis(gen), dis(gen), dis(gen));
//
//         rgb[0] = 200;
//         rgb[1] = 200;
//         rgb[2] = 200;
//         size = 10;
//         edgeSize = 1;
//     }
// };

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct Nodes {
    std::vector<std::string> titles;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
    std::vector<glm::vec3> forces;
    std::vector<Color> colors;
    std::vector<unsigned char> sizes;
    std::vector<unsigned char> edgeSizes;
    std::vector<bool> fixed;   // Whether this node's position is fixed (e.g., when dragged by user)
    std::vector<float> masses; // Node mass, affects force response
};

struct Edges {
    std::vector<uint32_t> startIdxs;
    std::vector<uint32_t> endIdxs;
};

// A graph storage class. Each Node is stored in an vectors, using the vector index as the indentifier. The edges store
// two values which reference to the start and end indexes of the nodes. A vector was chosen to store the Nodes and
// Edges over a Map as items will not regularly need to be deleted from the Graph.
// TODO: Template the graph to allow for different Node datastructures.
class Graph {
  public:
    Graph() {};
    ~Graph() = default;

    Graph &operator=(const Graph &other);

    uint32_t AddNode(std::string title);
    void AddEdge(uint32_t idx1, uint32_t idx2);
    void Clear();
    void AddDefaultData();

    std::vector<uint32_t> GetNeighboursIdx(uint32_t rootIdx) const;

    bool SaveBinary(const std::string &filename) const;
    bool LoadBinary(const std::string &filename);

    uint32_t GetTopNode();

    // Node &EdgeStart(uint32_t idx) { return nodes.at(edges.at(idx).startIdx); }
    // Node &EdgeEnd(uint32_t idx) { return nodes.at(edges.at(idx).endIdx); }

    Nodes nodes;
    Edges edges;
};

// A triple buffer system ensures that the node locations can be updated on the simulation thread, whilst also
// ensuring that data is always available to read from on the engine thread. A triple buffer was selected over a
// double buffer as the simulation thread needs to read from one graph copy and write to another. At the same time,
// the render engine can read from the third graph copy. More information can be found here.
// https://www.gamedev.net/forums/topic/403834-multithreading---sharing-data-triple-buffering/
// TODO: Re-evaluate a double buffer and compare.
class GraphTripleBuf {
  public:
    GraphTripleBuf();
    ~GraphTripleBuf();

    Graph *GetCurrent();
    Graph *GetWriteBuffer() { return m_buffers[m_write]; };
    void Publish();

    // Use this function instead of the usual Publish() function to write the graph data to all graph instances.
    // This is a slower operation and should only be used to initialise the state of the graph.
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
