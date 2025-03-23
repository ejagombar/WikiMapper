#ifndef DEBUGSIM_H
#define DEBUGSIM_H

struct debugData {
    float qqMultiplier = 0.2f;
    float gravityMultiplier = 10.f;
    float accelSizeMultiplier = 0.01f;
    float targetDistance = 67.f;
    bool resetSimulation = false;
    bool doneReset = false;
};

#endif
