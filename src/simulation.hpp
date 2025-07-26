#include "./graph.hpp"
#include "controlData.hpp"

#ifndef SIMULATION_H
#define SIMULATION_H

void updateGraphPositions(GS::Graph &writeG, const float dt, const SimulationControlData &simControlData);

#endif // SIMULATION_H
