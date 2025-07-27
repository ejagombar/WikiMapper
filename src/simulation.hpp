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

#endif // SIMULATION_H
