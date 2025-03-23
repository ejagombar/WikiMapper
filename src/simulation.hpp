#include "./graph.hpp"
#include "debugSim.hpp"

#ifndef SIMULATION_H
#define SIMULATION_H

void updateGraphPositions(const GS::Graph &readG, GS::Graph &writeG, const float dt, const debugData &simDebug);

#endif // SIMULATION_H
