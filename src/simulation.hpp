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
                std::shared_ptr<HttpInterface> &dBInterface, std::mutex &dBInterfaceMutex)
        : graphBuf(graphBuf), shouldTerminate(shouldTerminate), controlData(controlData), dBInterface(dBInterface),
          dBInterfaceMutex(dBInterfaceMutex) {};
    ~GraphEngine() {};

    void graphPositionSimulation();
    void setupGraph(GS::Graph &db, bool genData = true);

  private:
    struct PendingNodeExpansion {
        std::future<std::vector<LinkedPage>> future;
        uint32_t sourceNodeId;
        std::string nodeName;
    };

    std::optional<PendingNodeExpansion> pendingExpansion;

    GS::GraphTripleBuf &graphBuf;
    std::atomic<bool> &shouldTerminate;
    ControlData &controlData;

    std::shared_ptr<HttpInterface> &dBInterface;
    std::mutex &dBInterfaceMutex;

    void updateGraphPositions(GS::Graph &writeG, const float dt, const SimulationControlData &simControlData);
    void processControls(GS::Graph *readGraph, GS::Graph *writeGraph, SimulationControlData &dat);
    void generateRealData(GS::Graph &graph);
    void search(GS::Graph &graph, std::string query);
};

#endif // SIMULATION_H
