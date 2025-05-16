#ifndef DEBUGSIM_H
#define DEBUGSIM_H

#include <atomic>
#include <string>

struct SimulationControlData {
    bool resetSimulation = false;

    // Physics parameters
    float repulsionStrength = 800.0f; // Strength of repulsive force
    float attractionStrength = 0.2f;  // Strength of attractive force
    float centeringForce = 0.03f;     // Strength of centering force
    float timeStep = 0.5f;            // Integration time step scaling
    float forceMultiplier = 1.0f;
    float maxForce = 5.0f; // Force clamping to avoid instability
    float targetDistance = 37.0f;
    float maxVelocity = 2.0f;
};

struct GraphControlData {
    std::string searchString = "";
    std::atomic<bool> searching = false;
};

struct EngineControlData {
    std::atomic<bool> initGraphData = false;
    bool vSync = true;
    float cameraFov = 45;
    float mouseSensitivity = 2.f;

    float customColors[5];
};

struct ControlData {
    std::atomic<SimulationControlData> sim;
    GraphControlData graph;
    EngineControlData engine;
};

#endif
