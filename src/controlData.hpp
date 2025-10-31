#ifndef DEBUGSIM_H
#define DEBUGSIM_H

#include <atomic>
#include <cstdint>
#include <glm/glm.hpp>
#include <string>

struct SimParameters {
    float repulsionStrength = 6.0f;
    float attractionStrength = 100.f;
    float centeringForce = 40.f;
    float timeStep = 100.f;
    float forceMultiplier = 2.3f;
    float maxForce = 500.0f;
    float targetDistance = 1.0f;
    float maxVelocity = 2.0f;
};

struct DraggingNode {
    int32_t id = -1;
    glm::vec3 position{0, 0, 0};
};

struct SimulationControlData {
    bool resetSimulation = false;

    std::atomic<DraggingNode> draggingNode;

    std::atomic<SimParameters> parameters;
};

struct GraphControlData {
    std::string searchString = "";
    std::atomic<bool> searching = false;

    std::atomic<int32_t> sourceNode = -1;
};

struct EngineControlData {
    bool vSync = true;
    bool backgroundButtonToggle = false;
    float cameraFov = 45;
    float mouseSensitivity = 2.f;

    std::atomic<bool> initGraphData = false;

    float customVals[6] = {0.9f, 128.f, 0.2f, 0.007, 0.07, 0.017};
};

enum class dbInterfaceType { HTTP, DB };

struct DataSource {
    dbInterfaceType sourceType = dbInterfaceType::HTTP;

    std::string dbUrl = "http://localhost:7474";
    std::string dbUsername = "neo4j";
    std::string dbPassword = "test1234";

    std::string serverUrl = "http://eagombar.uk:6348";

    bool connectedToDataSource = false;
    bool attemptDataConnection = false;
};

struct ApplicationData {
    std::mutex dataSourceMutex;
    DataSource dataSource;

    int32_t background{0};
};

class ControlData {
  public:
    SimulationControlData sim;
    GraphControlData graph;
    EngineControlData engine;
    ApplicationData app;

    bool LoadControlData(const std::string &filename);
    bool SaveControlData(const std::string &filename);
};

#endif
