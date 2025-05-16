#ifndef GRAPH_H
#define GRAPH_H

#include <atomic>
#include <cstdint>
#include <cstring>
#include <glm/glm.hpp>
#include <random>
#include <vector>

namespace GS {

// The graph datastructure nodes are heavily linked to nodes that are displayed in the viewer. This allowed an
// intemediary layer to be removed from the engine, decreasing the number of copies, at the cost of modularity.
struct Node {
    char title[64];
    glm::vec3 pos = glm::vec3(0, 0, 0);
    glm::vec3 vel = glm::vec3(0, 0, 0);
    glm::vec3 force = glm::vec3(0, 0, 0);
    unsigned char rgb[3];
    unsigned char size;
    unsigned char edgeSize;
    bool fixed = false; // Whether this node's position is fixed (e.g., when dragged by user)
    float mass = 1.0f;  // Node mass, affects force response

    Node(const char *t) {
        strncpy(title, t, sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0'; // Ensure null-termination

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-100.0, 100.0);
        pos = glm::vec3(dis(gen), dis(gen), dis(gen));

        rgb[0] = 200;
        rgb[1] = 200;
        rgb[2] = 200;
        size = 10;
        edgeSize = 1;
    }
};

struct Edge {
    uint32_t startIdx;
    uint32_t endIdx;
};

// A graph storage class. Each Node is stored in an vectors, using the vector index as the indentifier. The edges store
// two values which reference to the start and end indexes of the nodes. A vector was chosen to store the Nodes and
// Edges over a Map as items will not regularly need to be deleted from the Graph.
// TODO: Template the graph to allow for different Node datastructures.
class Graph {
  public:
    Graph() {};
    ~Graph() {};

    Graph &operator=(const Graph &other);

    uint32_t AddNode(const char *title);
    void AddEdge(uint32_t idx1, uint32_t idx2);
    void Clear();

    std::vector<uint32_t> GetNeighboursIdx(uint32_t rootIdx) const;
    std::vector<Node> GetNeighbours(uint32_t rootIdx) const;

    float GetRadius() const;
    uint32_t GetTopNode();
    glm::vec3 GetCenter() const;

    Node &EdgeStart(uint32_t idx) { return nodes.at(edges.at(idx).startIdx); }
    Node &EdgeEnd(uint32_t idx) { return nodes.at(edges.at(idx).endIdx); }

    std::vector<Node> nodes;
    std::vector<Edge> edges;
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
