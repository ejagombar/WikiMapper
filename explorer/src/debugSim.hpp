#ifndef DEBUGSIM_H
#define DEBUGSIM_H

struct debugData {
    float qqMultiplier = 0.16f;
    float gravityMultiplier = 10.f;
    float accelSizeMultiplier = 0.001f;
    float targetDistance = 67.f;
    bool resetSimulation = false;
};

#endif
