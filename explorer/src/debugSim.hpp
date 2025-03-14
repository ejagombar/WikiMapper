#ifndef DEBUGSIM_H
#define DEBUGSIM_H

struct debugData {
    float qqMultiplier = 0.05f;
    float gravityMultiplier = 30.f;
    float accelSizeMultiplier = 0.01f;
    float targetDistance = 100;
    bool resetSimulation = false;
};

#endif
