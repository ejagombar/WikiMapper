#ifndef DEBUGSIM_H
#define DEBUGSIM_H

#include "../lib/mutex_guarded.hpp"
#include <string>

struct SimulationControlData {
    float forceMultiplier = 1.f;
    bool resetSimulation = false;

    // Debug Data Controls
    float qqMultiplier = 0.825f;
    float gravityMultiplier = 25.f;
    float accelSizeMultiplier = 0.001f;
    float targetDistance = 37.f;
};

struct GraphControlData {
    std::string searchString = "";
    bool searching = false;
};

struct EngineControlData {
    bool wireFrame = false;
    bool vSync = true;
    float cameraFov = 45;
    float mouseSensitivity = 2.f;
};

struct ControlData {
    mutex_guarded<SimulationControlData> sim;
    GraphControlData graph;
    EngineControlData engine;
};

#endif
