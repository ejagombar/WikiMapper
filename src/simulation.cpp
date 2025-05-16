#include <cmath>
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <json/json.h>
#include <vector>

#include "controlData.hpp"

#include "simulation.hpp"

void updateGraphPositions(const GS::Graph &readG, GS::Graph &writeG, const float dt,
                          const SimulationControlData &simControlData) {
    // Copy the graph state
    writeG = readG;

    // OSCILLATION DETECTION AND PREVENTION
    // Track node position history to detect oscillations
    static std::vector<glm::vec3> prevPositions;
    static std::vector<glm::vec3> prevMovements;
    static std::vector<float> nodeDamping;

    // Initialize or resize history arrays if needed
    if (prevPositions.size() != writeG.nodes.size()) {
        prevPositions.resize(writeG.nodes.size());
        prevMovements.resize(writeG.nodes.size(), glm::vec3(0));
        nodeDamping.resize(writeG.nodes.size(), 0.7f); // Default damping

        for (size_t i = 0; i < writeG.nodes.size(); i++) {
            prevPositions[i] = writeG.nodes[i].pos;
        }
    }

    // Reset forces
    for (auto &node : writeG.nodes) {
        node.force = glm::vec3(0, 0, 0);
    }

#ifdef DEBUG_FORCES
    std::cout << "Before update:" << std::endl;
    for (size_t i = 0; i < 3 && i < writeG.nodes.size(); i++) {
        std::cout << "Node " << i << " pos: (" << writeG.nodes[i].pos.x << ", " << writeG.nodes[i].pos.y << ", "
                  << writeG.nodes[i].pos.z << ")" << std::endl;
    }
#endif

    static float coolingFactor = 1.0f;
    coolingFactor = std::max(0.1f, coolingFactor * 0.99f);

    // Determine simulation scale based on node count to prevent explosion with large graphs
    float nodeCountScaling = 1.0f / (1.0f + std::log10(std::max(1.0f, float(writeG.nodes.size()))));

    float effectiveRepulsionStrength = simControlData.repulsionStrength * nodeCountScaling * coolingFactor;
    float effectiveAttractionStrength = simControlData.attractionStrength * coolingFactor;
    float effectiveCenteringForce = simControlData.centeringForce * 0.1f;

    float safeTimeStep = dt * simControlData.timeStep * 0.1f;

    for (size_t i = 0; i < writeG.nodes.size(); i++) {
        glm::vec3 currentMovement = writeG.nodes[i].pos - prevPositions[i];

        float movementDot = glm::dot(currentMovement, prevMovements[i]);
        float currentMagnitude = glm::length(currentMovement);
        float prevMagnitude = glm::length(prevMovements[i]);

        if (currentMagnitude > 0.01f && prevMagnitude > 0.01f) {
            if (movementDot < 0) {
                float reversalStrength = -movementDot / (currentMagnitude * prevMagnitude + 0.0001f);

                nodeDamping[i] = std::min(0.95f, nodeDamping[i] + reversalStrength * 0.2f);

                // For severe oscillations, apply position smoothing
                if (reversalStrength > 0.7f && currentMagnitude > 0.5f) {
                    // Move halfway back toward previous position to dampen oscillation
                    writeG.nodes[i].pos = writeG.nodes[i].pos * 0.7f + prevPositions[i] * 0.3f;
                    writeG.nodes[i].vel *= 0.5f; // Reduce velocity
                }
            } else {
                // Gradually decrease damping for non-oscillating nodes
                nodeDamping[i] = std::max(0.5f, nodeDamping[i] - 0.01f);
            }
        }

        // Store current position and movement for next iteration
        prevMovements[i] = currentMovement;
        prevPositions[i] = writeG.nodes[i].pos;
    }

    for (size_t i = 0; i < writeG.nodes.size(); i++) {
        if (writeG.nodes[i].fixed)
            continue;

        std::vector<uint32_t> neighbors = writeG.GetNeighboursIdx(i);
        int neighborCount = neighbors.size();

        for (size_t j = 0; j < writeG.nodes.size(); j++) {
            if (i == j)
                continue;

            glm::vec3 delta = writeG.nodes[i].pos - writeG.nodes[j].pos;
            float distSq = glm::dot(delta, delta);

            const float safeMinDistance = 5.0f; // Much larger minimum distance
            if (distSq < safeMinDistance * safeMinDistance) {
                distSq = safeMinDistance * safeMinDistance;
            }

            float repulsionForce = effectiveRepulsionStrength * writeG.nodes[i].size * writeG.nodes[j].size / distSq;

            repulsionForce = std::min(repulsionForce, 1.0f);

            if (glm::dot(delta, delta) > 1e-10f) {
                writeG.nodes[i].force += glm::normalize(delta) * repulsionForce;
            }
        }

        glm::vec3 totalAttractionForce(0, 0, 0);

        for (uint32_t neighborIdx : neighbors) {
            glm::vec3 delta = writeG.nodes[neighborIdx].pos - writeG.nodes[i].pos;
            float distance = glm::length(delta);

            if (distance < 0.1f)
                continue;

            float connectionFactor = 1.0f / std::sqrt(std::max(1, neighborCount));

            float attractionForce =
                effectiveAttractionStrength * connectionFactor * (distance - simControlData.targetDistance);

            attractionForce = std::max(-1.0f, std::min(1.0f, attractionForce));

            if (distance > 1e-10f) {
                glm::vec3 attractForce = glm::normalize(delta) * attractionForce;
                writeG.nodes[i].force += attractForce;
                totalAttractionForce += attractForce;
            }
        }

        float centeringScale = 1.0f / (1.0f + neighborCount * 0.2f);

        glm::vec3 center = glm::vec3(0, 0, 0); // Use origin as center instead of graph center
        glm::vec3 toCenter = center - writeG.nodes[i].pos;
        float distanceToCenter = glm::length(toCenter);

        if (distanceToCenter > 50.0f) {
            float centeringStrength = effectiveCenteringForce * centeringScale * (distanceToCenter - 50.0f) / 50.0f;
            writeG.nodes[i].force += glm::normalize(toCenter) * centeringStrength;
        }

        writeG.nodes[i].force *= simControlData.forceMultiplier;

        float totalAttractionMagnitude = glm::length(totalAttractionForce);
        float forceMagnitude = glm::length(writeG.nodes[i].force);

        if (totalAttractionMagnitude > forceMagnitude * 0.8f && totalAttractionMagnitude > 0.5f) {
            float reductionFactor = (forceMagnitude * 0.8f) / totalAttractionMagnitude;
            writeG.nodes[i].force -= totalAttractionForce * (1.0f - reductionFactor);
        }

        forceMagnitude = glm::length(writeG.nodes[i].force);
        if (forceMagnitude > simControlData.maxForce) {
            writeG.nodes[i].force = (writeG.nodes[i].force / forceMagnitude) * simControlData.maxForce;
        }

        if (std::isnan(writeG.nodes[i].force.x) || std::isnan(writeG.nodes[i].force.y) ||
            std::isnan(writeG.nodes[i].force.z)) {
            writeG.nodes[i].force = glm::vec3(0, 0, 0); // Reset NaN forces
#ifdef DEBUG_FORCES
            std::cout << "NaN force detected for node " << i << std::endl;
#endif
        }
    }

    for (size_t i = 0; i < writeG.nodes.size(); i++) {
        auto &node = writeG.nodes[i];
        if (node.fixed)
            continue;

        float nodeDampingFactor = nodeDamping[i];

        glm::vec3 acceleration = node.force / std::max(0.1f, node.mass);

        node.vel = node.vel * nodeDampingFactor + acceleration * safeTimeStep;

        if (nodeDampingFactor > 0.8f) { // Node is likely oscillating
            // Blend new velocity with average of old velocities for smoother transitions
            static std::vector<glm::vec3> prevVelocities;
            if (prevVelocities.size() != writeG.nodes.size()) {
                prevVelocities.resize(writeG.nodes.size(), glm::vec3(0));
            }

            node.vel = node.vel * 0.6f + prevVelocities[i] * 0.4f;
            prevVelocities[i] = node.vel;
        }

        float velocityMagnitude = glm::length(node.vel);
        if (velocityMagnitude > simControlData.maxVelocity) {
            node.vel = (node.vel / velocityMagnitude) * simControlData.maxVelocity;
        }

        if (std::isnan(node.vel.x) || std::isnan(node.vel.y) || std::isnan(node.vel.z)) {
            node.vel = glm::vec3(0, 0, 0);
        }

        glm::vec3 oldPos = node.pos;

        node.pos += node.vel * safeTimeStep;

        // Position change limiting - prevent large jumps
        float positionChange = glm::length(node.pos - oldPos);
        float maxPositionChange = 2.0f;
        if (positionChange > maxPositionChange) {
            node.pos = oldPos + ((node.pos - oldPos) / positionChange) * maxPositionChange;
            node.vel *= 0.7f;
        }

        // adaptive boundary handling - soft boundaries that push back
        for (int d = 0; d < 3; d++) {
            if (std::isnan(node.pos[d])) {
                node.pos[d] = 0;
                node.vel[d] = 0;
            } else if (node.pos[d] > 200.0f) {
                // Soft boundary - pushes back with increasing force
                float excess = node.pos[d] - 200.0f;
                node.pos[d] = 200.0f - 5.0f * (1.0f - std::exp(-excess * 0.1f));
                node.vel[d] *= -0.5f; // Reverse and dampen velocity
            } else if (node.pos[d] < -200.0f) {
                float excess = -200.0f - node.pos[d];
                node.pos[d] = -200.0f + 5.0f * (1.0f - std::exp(-excess * 0.1f));
                node.vel[d] *= -0.5f; // Reverse and dampen velocity
            }
        }
    }

#ifdef DEBUG_FORCES
    std::cout << "After update:" << std::endl;
    for (size_t i = 0; i < 3 && i < writeG.nodes.size(); i++) {
        std::cout << "Node " << i << " pos: (" << writeG.nodes[i].pos.x << ", " << writeG.nodes[i].pos.y << ", "
                  << writeG.nodes[i].pos.z << ")" << std::endl;
    }
#endif

    // Reset simulation if requested
    if (simControlData.resetSimulation) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-10.0, 10.0); // Start much closer to center

        for (auto &node : writeG.nodes) {
            // Reset to random positions around origin with small values
            node.pos = glm::vec3(dis(gen), dis(gen), dis(gen));
            node.vel = glm::vec3(0, 0, 0);
            node.force = glm::vec3(0, 0, 0);
            node.fixed = false;
        }

        // Reset history data
        for (size_t i = 0; i < writeG.nodes.size(); i++) {
            prevPositions[i] = writeG.nodes[i].pos;
            prevMovements[i] = glm::vec3(0);
            nodeDamping[i] = 0.7f;
        }

        // Reset the cooling factor when simulation is reset
        coolingFactor = 1.0f;
    }
}
