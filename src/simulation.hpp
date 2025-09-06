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
    static constexpr float THETA = 0.7f;
    static constexpr float MIN_DISTANCE_SQ = 0.01f;
    static constexpr int MAX_DEPTH = 25;

    struct Node {
        glm::vec3 centerOfMass;
        float totalMass;
        glm::vec3 minBounds;
        glm::vec3 maxBounds;

        std::array<int32_t, 8> children;
        int32_t bodyIndex;
        uint32_t bodyCount;

        Node() : centerOfMass(0.0f), totalMass(0.0f), minBounds(0.0f), maxBounds(0.0f), bodyIndex(-1), bodyCount(0) {
            std::fill(children.begin(), children.end(), -1);
        }
    };

    BarnesHutTree();
    ~BarnesHutTree() = default;

    void clear();
    void verifyTree() const;
    void build(const std::vector<glm::vec3> &positions, const std::vector<float> &masses);
    glm::vec3 calculateForce(size_t bodyIndex, const std::vector<glm::vec3> &positions,
                             const std::vector<float> &masses, const std::vector<unsigned char> &sizes,
                             float repulsionStrength) const;

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

    int32_t allocateNode();
    static int getOctant(const glm::vec3 &pos, const glm::vec3 &center);
    void insertBody(int32_t nodeIndex, int32_t bodyIndex, const glm::vec3 &pos, float mass, int depth);
    glm::vec3 calculateForceRecursive(int32_t nodeIndex, const glm::vec3 &bodyPos, float bodyMass, float bodySizeSq,
                                      float repulsionStrength) const;
    bool useNodeAsSingleBody(const Node &node, const glm::vec3 &bodyPos) const;
    void computeBounds(const std::vector<glm::vec3> &positions, glm::vec3 &minBounds, glm::vec3 &maxBounds) const;
};

#endif // SIMULATION_H
