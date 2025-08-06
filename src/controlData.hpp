#ifndef DEBUGSIM_H
#define DEBUGSIM_H

#include <atomic>
#include <cstdint>
#include <string>

struct SimulationControlData {
    bool resetSimulation = false;

    // Physics parameters
    float repulsionStrength = 4.0f;  // Strength of repulsive force
    float attractionStrength = 65.f; // Strength of attractive force
    float centeringForce = 40.f;     // Strength of centering force
    float timeStep = 200.f;          // Integration time step scaling
    float forceMultiplier = 1.0f;
    float maxForce = 5.0f; // Force clamping to avoid instability
    float targetDistance = 11.0f;
    float maxVelocity = 2.0f;
    int32_t fixedNode = -1;
};

struct GraphControlData {
    std::string searchString = "";
    std::atomic<bool> searching = false;

    std::atomic<int32_t> sourceNode = -1;
};

struct EngineControlData {
    std::atomic<bool> initGraphData = false;
    bool vSync = true;
    float cameraFov = 45;
    float mouseSensitivity = 2.f;

    float customVals[6] = {0.9f, 128.f, 0.2f, 0.007, 0.07, 0.017};
};

struct ControlData {
    std::atomic<SimulationControlData> sim;
    GraphControlData graph;
    EngineControlData engine;
};

#endif
