#include "./graph.hpp"
#include "controlData.hpp"

#ifndef SIMULATION_H
#define SIMULATION_H

void updateGraphPositions(const GS::Graph &readG, GS::Graph &writeG, const float dt,
                          const SimulationControlData &simControlData);

#endif // SIMULATION_H
