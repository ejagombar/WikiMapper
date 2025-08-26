#include "./graph.hpp"
#include "controlData.hpp"
#include "store.hpp"
#include <future>
#include <memory>
#include <optional>

#ifndef SIMULATION_H
#define SIMULATION_H

class GraphEngine {
  public:
    GraphEngine(GS::GraphTripleBuf &graphBuf, std::atomic<bool> &shouldTerminate, ControlData &controlData,
                std::shared_ptr<dBInterface> &dBInterface, std::mutex &dBInterfaceMutex)
        : m_graphBuf(graphBuf), m_shouldTerminate(shouldTerminate), m_controlData(controlData), m_dB(dBInterface),
          m_dBInterfaceMutex(dBInterfaceMutex) {};
    ~GraphEngine() {};

    void graphPositionSimulation();
    void setupGraph(GS::Graph &db, bool genData = true);

  private:
    struct PendingNodeExpansion {
        std::future<std::vector<LinkedPage>> m_future;
        uint32_t m_sourceNodeId;
        std::string m_nodeName;
    };

    std::optional<PendingNodeExpansion> m_pendingExpansion;

    GS::GraphTripleBuf &m_graphBuf;
    std::atomic<bool> &m_shouldTerminate;
    ControlData &m_controlData;

    std::shared_ptr<dBInterface> &m_dB;
    std::mutex &m_dBInterfaceMutex;

    void updateGraphPositions(GS::Graph &writeG, const float dt, const SimulationControlData &simControlData);
    void processControls(GS::Graph *readGraph, GS::Graph *writeGraph, SimulationControlData &dat);
    void generateRealData(GS::Graph &graph);
    void search(GS::Graph &graph, std::string query);
};

class BarnesHutTree {
  public:
    static constexpr float THETA = 0.8f;
    static constexpr float MIN_DISTANCE_SQ = 0.1f;
    static constexpr int MAX_DEPTH = 20;

    struct Node {
        glm::vec3 centerOfMass;
        float totalMass;
        glm::vec3 minBounds;
        glm::vec3 maxBounds;

        // Child indices in the node pool (-1 if none)
        std::array<int32_t, 8> children;

        // Body index if this is a leaf node (-1 if internal node)
        int32_t bodyIndex;

        // Number of bodies in this node
        uint32_t bodyCount;

        Node() : centerOfMass(0.0f), totalMass(0.0f), minBounds(0.0f), maxBounds(0.0f), bodyIndex(-1), bodyCount(0) {
            std::fill(children.begin(), children.end(), -1);
        }
    };

    BarnesHutTree();
    ~BarnesHutTree() = default;

    // Build the tree from positions and masses
    void build(const std::vector<glm::vec3> &positions, const std::vector<float> &masses);

    // Calculate force on a body using Barnes-Hut algorithm
    glm::vec3 calculateForce(size_t bodyIndex, const std::vector<glm::vec3> &positions,
                             const std::vector<float> &masses, const std::vector<unsigned char> &sizes,
                             float repulsionStrength) const;

    // Clear the tree
    void clear();

    // Get tree statistics for debugging/optimization
    struct Stats {
        size_t nodeCount;
        size_t maxDepth;
        float averageBodiesPerLeaf;
    };
    Stats getStats() const;

  private:
    std::vector<Node> m_nodes;
    int32_t m_rootIndex;
    size_t m_nodeCount;

    // Node pool management
    int32_t allocateNode();

    // Octree child index calculation
    static int getOctant(const glm::vec3 &pos, const glm::vec3 &center);

    // Recursive tree building
    void insertBody(int32_t nodeIndex, size_t bodyIndex, const glm::vec3 &pos, float mass, int depth);

    // Recursive force calculation
    glm::vec3 calculateForceRecursive(int32_t nodeIndex, const glm::vec3 &bodyPos, float bodyMass, float bodySizeSq,
                                      float repulsionStrength) const;

    // Helper to check if we should use node as single body (Barnes-Hut criterion)
    bool useNodeAsSingleBody(const Node &node, const glm::vec3 &bodyPos) const;

    // Compute bounding box for initial tree setup
    void computeBounds(const std::vector<glm::vec3> &positions, glm::vec3 &minBounds, glm::vec3 &maxBounds) const;
};

class ForceAccumulator {
  public:
    ForceAccumulator(size_t capacity);

    void reset();
    void accumulate(size_t index, const glm::vec3 &force);
    void applyTo(std::vector<glm::vec3> &forces) const;

  private:
    struct alignas(16) AlignedVec3 {
        float x, y, z, pad;
    };

    std::vector<AlignedVec3> m_forces;
};

#endif // SIMULATION_H
