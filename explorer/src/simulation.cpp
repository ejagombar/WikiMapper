#include <cmath>
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <json/json.h>
#include <vector>

#include "simulation.hpp"

bool checkValid(const float &f) { return !(std::isnan(f) || std::isinf(f)); }
bool checkValid(const glm::vec3 &v) { return checkValid(v.x) && checkValid(v.y) && checkValid(v.z); }

void updateGraphPositions(const GS::Graph &readG, GS::Graph &writeG, const float dt, const debugData &simDebug) {
    const float qqMultiplier = simDebug.qqMultiplier;
    const float gravityMultiplier = simDebug.gravityMultiplier;
    const float accelSizeMultiplier = simDebug.accelSizeMultiplier;
    const float targetDistance = simDebug.targetDistance;
    const float edgeForceMultiplier = 0.01;

    const uint32_t nodeCount = readG.nodes.size();

    std::vector<glm::vec3> nodeForces(nodeCount, glm::vec3(0));

    for (const GS::Edge &edge : readG.edges) {
        const glm::vec3 delta = readG.nodes[edge.startIdx].pos - readG.nodes[edge.endIdx].pos;
        const float distance = glm::length(delta);

        const glm::vec3 direction = delta / distance;
        const float distanceDelta = distance - targetDistance;

        if (distanceDelta < 0.01) {
            continue;
        }

        float attractiveForce = distanceDelta * distanceDelta * edgeForceMultiplier;

        // if (attractiveForce > 1000.f) {
        //     attractiveForce = 1000.f;
        // }
        //
        // if (attractiveForce < -1000.f) {
        //     attractiveForce = -1000.f;
        // }

        const glm::vec3 forceVec = direction * attractiveForce;

        nodeForces[edge.endIdx] += forceVec;
        nodeForces[edge.startIdx] -= forceVec;
    }

    for (uint32_t i = 0; i < nodeCount; i++) {
        // Apply gravity towards (0,0,0)
        // float distance = glm::length(readG.nodes[i].pos);
        glm::vec3 gravityForce = glm::normalize(readG.nodes[i].pos) * gravityMultiplier;
        if (checkValid(gravityForce)) {
            nodeForces[i] -= gravityForce;
        }

        // Apply node-node repulsion using coulomb's force
        const GS::Node &node1 = readG.nodes[i];

        glm::vec3 force = glm::vec3(0);
        for (uint32_t j = 0; j < nodeCount; j++) {
            const GS::Node &node2 = readG.nodes[j];

            const float qq = node1.size * node2.size;
            float distance = glm::distance(node1.pos, node2.pos);
            if (distance < 0.01) {
                distance = 0.01;
            }

            float electrostaticForce = (qqMultiplier * qq) / (distance * distance);
            // if (electrostaticForce > 1000) {
            //     electrostaticForce = 1000;
            // }

            glm::vec3 direction = glm::normalize(node1.pos - node2.pos);
            if (checkValid(direction)) {
                force += direction * electrostaticForce;
            }
        }

        nodeForces[i] += force;

        // Apply forces and update velocity, position
        // if (glm::length(nodeForces[i]) < 0.1) {
        //     nodeForces[i] = glm::vec3(0);
        // }

        const glm::vec3 acceleration = (nodeForces[i] * (1.0f / readG.nodes[i].size)) * accelSizeMultiplier;
        const glm::vec3 vel = acceleration * dt;

        if (checkValid(nodeForces[i])) {
            writeG.nodes[i].force = readG.nodes[i].force + nodeForces[i];
        }
        if (checkValid(vel)) {
            writeG.nodes[i].vel = readG.nodes[i].vel + vel;
            writeG.nodes[i].pos = readG.nodes[i].pos + vel * dt;
        }

        // if (i == 2) {
        //     std::cout << "Force: " << nodeForces[i].x << "," << nodeForces[i].y << "," << nodeForces[i].z <<
        //     std::endl; std::cout << "Acceleration: " << acceleration.x << "," << acceleration.y << "," <<
        //     acceleration.z
        //               << std::endl;
        //     std::cout << "Vel: " << vel.x << "," << vel.y << "," << vel.z << std::endl;
        //
        //     std::cout << "W Position: " << readG.nodes[i].pos.x << "," << readG.nodes[i].pos.y << ","
        //               << readG.nodes[i].pos.z << std::endl;
        //     std::cout << "W Acceleration: " << readG.nodes[i].force.x << "," << readG.nodes[i].force.y << ","
        //               << readG.nodes[i].force.z << std::endl;
        //     std::cout << "W Vel: " << readG.nodes[i].vel.x << "," << readG.nodes[i].vel.y << "," <<
        //     readG.nodes[i].vel.z
        //               << std::endl;
        // }
    }
}
