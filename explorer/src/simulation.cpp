#include <cmath>
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <json/json.h>
#include <vector>

#include "simulation.hpp"

void updateGraphPositions(const GS::Graph &readG, GS::Graph &writeG, const float dt, const debugData &simDebug) {

    const float qqMultiplier = simDebug.qqMultiplier;
    const float gravityMultiplier = simDebug.gravityMultiplier;
    const float accelSizeMultiplier = simDebug.accelSizeMultiplier;
    const float targetDistance = simDebug.targetDistance;

    const uint nodeCount = readG.nodes.size();

    std::vector<glm::vec3> nodeForces(nodeCount, glm::vec3(0));

    for (const GS::Edge &edge : readG.edges) {
        glm::vec3 delta = readG.nodes[edge.startIdx].pos - readG.nodes[edge.endIdx].pos;
        float distance = glm::length(delta);

        const glm::vec3 direction = delta / distance;
        const float distanceDelta = distance - targetDistance;

        float attractiveForce = distanceDelta * distanceDelta * 0.01;

        if (attractiveForce > 1000.f) {
            attractiveForce = 1000.f;
        }

        if (attractiveForce < -1000.f) {
            attractiveForce = -1000.f;
        }

        const glm::vec3 forceVec = direction * attractiveForce;

        nodeForces[edge.endIdx] += forceVec;
        nodeForces[edge.startIdx] -= forceVec;
    }

    for (uint i = 0; i < nodeCount; i++) {
        // Apply gravity towards (0,0,0)
        if (glm::length(readG.nodes[i].pos) != 0) [[likely]] {
            nodeForces[i] -= glm::normalize(readG.nodes[i].pos) * gravityMultiplier;
        }

        // Apply node-node repulsion using coulomb's force
        const GS::Node &node1 = readG.nodes[i];

        // if (i == 0) {
        //     std::cout << "2" << nodeForces[i].x << " " << nodeForces[i].y << " " << nodeForces[i].z << std::endl;
        // }

        glm::vec3 force = glm::vec3(0);
        for (uint j = 0; j < nodeCount; j++) {
            const GS::Node &node2 = readG.nodes[j];

            if (node1.pos == node2.pos) { // Divide by zero bad
                continue;
            }

            const float qq = node1.size * node2.size;
            const float distance = glm::distance(node1.pos, node2.pos);

            const float electrostaticForce = (qqMultiplier * qq) / (distance * distance);
            force += glm::normalize(node1.pos - node2.pos) * electrostaticForce;
        }
        nodeForces[i] += force;

        // Apply forces and update velocity, position
        if (glm::length(nodeForces[i]) < 0.4) {
            nodeForces[i] = glm::vec3(0);
        }

        const glm::vec3 acceleration = (nodeForces[i] * (1.0f / readG.nodes[i].size)) * accelSizeMultiplier;
        const glm::vec3 vel = acceleration * dt;

        writeG.nodes[i].force = readG.nodes[i].force * 0.95f + nodeForces[i];
        writeG.nodes[i].vel = readG.nodes[i].vel + vel;
        writeG.nodes[i].pos = readG.nodes[i].pos + vel * dt;
    }

    // writeG.nodes[0].pos = glm::vec3(0);
}
