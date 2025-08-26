#include "../lib/rgb_hsv.hpp"
#include "pointMaths.hpp"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <future>
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <json/json.h>
#include <optional>
#include <random>
#include <vector>

#include "controlData.hpp"
#include "logger.hpp"

#include "simulation.hpp"

// #define DEBUG_FORCES

BarnesHutTree::BarnesHutTree() : m_rootIndex(-1), m_nodeCount(0) { m_nodes.reserve(1000); }

void BarnesHutTree::clear() {
    m_nodes.clear();
    m_rootIndex = -1;
    m_nodeCount = 0;
}

int32_t BarnesHutTree::allocateNode() {
    int32_t index = static_cast<int32_t>(m_nodes.size());
    m_nodes.emplace_back();
    m_nodeCount++;
    return index;
}

int BarnesHutTree::getOctant(const glm::vec3 &pos, const glm::vec3 &center) {
    int octant = 0;
    if (pos.x > center.x)
        octant |= 1;
    if (pos.y > center.y)
        octant |= 2;
    if (pos.z > center.z)
        octant |= 4;
    return octant;
}

void BarnesHutTree::computeBounds(const std::vector<glm::vec3> &positions, glm::vec3 &minBounds,
                                  glm::vec3 &maxBounds) const {
    if (positions.empty()) {
        minBounds = glm::vec3(-100.0f);
        maxBounds = glm::vec3(100.0f);
        return;
    }

    minBounds = positions[0];
    maxBounds = positions[0];

// Vectorization hint for compiler
#pragma omp simd reduction(min : minBounds.x, minBounds.y, minBounds.z)                                                \
    reduction(max : maxBounds.x, maxBounds.y, maxBounds.z)
    for (size_t i = 1; i < positions.size(); ++i) {
        minBounds.x = std::min(minBounds.x, positions[i].x);
        minBounds.y = std::min(minBounds.y, positions[i].y);
        minBounds.z = std::min(minBounds.z, positions[i].z);

        maxBounds.x = std::max(maxBounds.x, positions[i].x);
        maxBounds.y = std::max(maxBounds.y, positions[i].y);
        maxBounds.z = std::max(maxBounds.z, positions[i].z);
    }

    // Add small margin to avoid edge cases
    glm::vec3 margin = (maxBounds - minBounds) * 0.01f + glm::vec3(1.0f);
    minBounds -= margin;
    maxBounds += margin;
}

void BarnesHutTree::build(const std::vector<glm::vec3> &positions, const std::vector<float> &masses) {
    clear();

    if (positions.empty())
        return;

    // Compute bounding box
    glm::vec3 minBounds, maxBounds;
    computeBounds(positions, minBounds, maxBounds);

    // Create root node
    m_rootIndex = allocateNode();
    Node &root = m_nodes[m_rootIndex];
    root.minBounds = minBounds;
    root.maxBounds = maxBounds;

    // Insert all bodies
    for (size_t i = 0; i < positions.size(); ++i) {
        insertBody(m_rootIndex, i, positions[i], masses[i], 0);
    }
}

void BarnesHutTree::insertBody(int32_t nodeIndex, size_t bodyIndex, const glm::vec3 &pos, float mass, int depth) {
    if (depth > MAX_DEPTH)
        return; // Prevent infinite recursion

    Node &node = m_nodes[nodeIndex];

    // Update center of mass (incremental)
    float newTotalMass = node.totalMass + mass;
    if (newTotalMass > 0) {
        node.centerOfMass = (node.centerOfMass * node.totalMass + pos * mass) / newTotalMass;
        node.totalMass = newTotalMass;
    }
    node.bodyCount++;

    // If this is an empty leaf, store the body
    if (node.bodyCount == 1) {
        node.bodyIndex = static_cast<int32_t>(bodyIndex);
        return;
    }

    // If this was a leaf with one body, we need to subdivide
    if (node.bodyIndex >= 0) {
        // Store the existing body temporarily
        int32_t existingBodyIndex = node.bodyIndex;
        glm::vec3 existingPos = node.centerOfMass;
        float existingMass = node.totalMass - mass; // Remove the new body's contribution

        // Convert to internal node
        node.bodyIndex = -1;

        // Reinsert existing body
        glm::vec3 center = (node.minBounds + node.maxBounds) * 0.5f;
        int existingOctant = getOctant(existingPos, center);

        if (node.children[existingOctant] < 0) {
            node.children[existingOctant] = allocateNode();
            Node &child = m_nodes[node.children[existingOctant]];

            // Calculate child bounds
            for (int i = 0; i < 3; ++i) {
                if (existingOctant & (1 << i)) {
                    child.minBounds[i] = center[i];
                    child.maxBounds[i] = node.maxBounds[i];
                } else {
                    child.minBounds[i] = node.minBounds[i];
                    child.maxBounds[i] = center[i];
                }
            }
        }

        // Reinsert with correct mass (without the new body)
        node.centerOfMass = existingPos;
        node.totalMass = existingMass;
        node.bodyCount = 1;
        insertBody(node.children[existingOctant], existingBodyIndex, existingPos, existingMass, depth + 1);
    }

    // Insert new body into appropriate child
    glm::vec3 center = (node.minBounds + node.maxBounds) * 0.5f;
    int octant = getOctant(pos, center);

    if (node.children[octant] < 0) {
        node.children[octant] = allocateNode();
        Node &child = m_nodes[node.children[octant]];

        // Calculate child bounds
        for (int i = 0; i < 3; ++i) {
            if (octant & (1 << i)) {
                child.minBounds[i] = center[i];
                child.maxBounds[i] = node.maxBounds[i];
            } else {
                child.minBounds[i] = node.minBounds[i];
                child.maxBounds[i] = center[i];
            }
        }
    }

    insertBody(node.children[octant], bodyIndex, pos, mass, depth + 1);
}

bool BarnesHutTree::useNodeAsSingleBody(const Node &node, const glm::vec3 &bodyPos) const {
    if (node.bodyCount <= 1)
        return true;

    glm::vec3 size = node.maxBounds - node.minBounds;
    float maxSize = std::max({size.x, size.y, size.z});

    glm::vec3 delta = node.centerOfMass - bodyPos;
    float distanceSq = glm::dot(delta, delta);

    // Barnes-Hut criterion: s/d < theta
    // where s is the size of the region and d is the distance
    float ratio = (maxSize * maxSize) / distanceSq;
    return ratio < (THETA * THETA);
}

glm::vec3 BarnesHutTree::calculateForceRecursive(int32_t nodeIndex, const glm::vec3 &bodyPos, float bodyMass,
                                                 float bodySizeSq, float repulsionStrength) const {
    if (nodeIndex < 0)
        return glm::vec3(0.0f);

    const Node &node = m_nodes[nodeIndex];

    if (node.bodyCount == 0)
        return glm::vec3(0.0f);

    glm::vec3 delta = node.centerOfMass - bodyPos;
    float distSq = glm::dot(delta, delta);

    float softening = 0.5f;
    distSq += softening * softening;

    // Skip if this is the same body (for leaf nodes)
    if (distSq < MIN_DISTANCE_SQ && node.bodyCount == 1) {
        return glm::vec3(0.0f);
    }

    // Use Barnes-Hut criterion
    if (useNodeAsSingleBody(node, bodyPos)) {
        // Treat as single body
        distSq = std::max(distSq, MIN_DISTANCE_SQ);

        float repulsionForce = repulsionStrength * bodySizeSq * node.totalMass / distSq;
        float invDist = 1.0f / std::sqrt(distSq);

        // Force points away from the other body
        return -delta * (repulsionForce * invDist);
    } else {
        // Recursively calculate forces from children
        glm::vec3 totalForce(0.0f);

// Unroll loop for better vectorization
#pragma unroll
        for (int i = 0; i < 8; ++i) {
            if (node.children[i] >= 0) {
                totalForce +=
                    calculateForceRecursive(node.children[i], bodyPos, bodyMass, bodySizeSq, repulsionStrength);
            }
        }

        return totalForce;
    }
}

glm::vec3 BarnesHutTree::calculateForce(size_t bodyIndex, const std::vector<glm::vec3> &positions,
                                        const std::vector<float> &masses, const std::vector<unsigned char> &sizes,
                                        float repulsionStrength) const {
    if (m_rootIndex < 0)
        return glm::vec3(0.0f);

    const glm::vec3 &pos = positions[bodyIndex];
    float mass = masses[bodyIndex];
    float sizeSq = sizes[bodyIndex] * sizes[bodyIndex];

    return calculateForceRecursive(m_rootIndex, pos, mass, sizeSq, repulsionStrength);
}

BarnesHutTree::Stats BarnesHutTree::getStats() const {
    Stats stats;
    stats.nodeCount = m_nodeCount;
    stats.maxDepth = 0;
    stats.averageBodiesPerLeaf = 0;

    // Calculate stats if needed for debugging

    return stats;
}

// ForceAccumulator implementation

ForceAccumulator::ForceAccumulator(size_t capacity) {
    m_forces.resize(capacity);
    reset();
}

void ForceAccumulator::reset() {
    // Vectorized memset
    std::memset(m_forces.data(), 0, m_forces.size() * sizeof(AlignedVec3));
}

void ForceAccumulator::accumulate(size_t index, const glm::vec3 &force) {
    m_forces[index].x += force.x;
    m_forces[index].y += force.y;
    m_forces[index].z += force.z;
}

void ForceAccumulator::applyTo(std::vector<glm::vec3> &forces) const {
    size_t count = std::min(forces.size(), m_forces.size());

// Vectorized copy with compiler hints
#pragma omp simd
    for (size_t i = 0; i < count; ++i) {
        forces[i].x += m_forces[i].x;
        forces[i].y += m_forces[i].y;
        forces[i].z += m_forces[i].z;
    }
}

void updateGraphPositionsBarnesHut(GS::Graph &writeG, float dt, const SimulationControlData &simControlData) {
    static std::vector<glm::vec3> prevPositions;
    static std::vector<glm::vec3> prevMovements;
    static std::vector<float> nodeDamping;
    static BarnesHutTree bhTree;
    static ForceAccumulator forceAccum(10000);

    const auto parameters = simControlData.parameters.load(std::memory_order_relaxed);
    const auto draggingNode = simControlData.draggingNode.load(std::memory_order_relaxed);

    size_t nodeCount = writeG.nodes.positions.size();

    if (prevPositions.size() != nodeCount) {
        prevPositions.resize(nodeCount);
        prevMovements.resize(nodeCount, glm::vec3(0));
        nodeDamping.resize(nodeCount, 0.7f);

        prevPositions = writeG.nodes.positions;
    }

    // Reset forces
    std::fill(writeG.nodes.forces.begin(), writeG.nodes.forces.end(), glm::vec3(0, 0, 0));
    forceAccum.reset();

    static float coolingFactor = 1.0f;
    // coolingFactor = std::max(0.0f, coolingFactor * 0.999f);

    float nodeCountScaling = 1.0f / (1.0f + std::log10(std::max(1.0f, float(nodeCount))));

    float effectiveRepulsionStrength = parameters.repulsionStrength * nodeCountScaling * coolingFactor;
    float effectiveAttractionStrength = parameters.attractionStrength * coolingFactor;
    float effectiveCenteringForce = parameters.centeringForce * 0.1f;

    float safeTimeStep = dt * parameters.timeStep;

    bhTree.build(writeG.nodes.positions, writeG.nodes.masses);

#pragma omp simd
    for (size_t i = 0; i < nodeCount; ++i) {
        writeG.nodes.forces[i] += bhTree.calculateForce(i, writeG.nodes.positions, writeG.nodes.masses,
                                                        writeG.nodes.sizes, effectiveRepulsionStrength);
    }

    for (size_t i = 0; i < nodeCount; ++i) {
        if (static_cast<int32_t>(i) == draggingNode.id)
            continue;

        glm::vec3 currentMovement = writeG.nodes.positions[i] - prevPositions[i];
        float movementDot = glm::dot(currentMovement, prevMovements[i]);
        float currentMagnitude = glm::length(currentMovement);
        float prevMagnitude = glm::length(prevMovements[i]);

        if (currentMagnitude > 0.01f && prevMagnitude > 0.01f) {
            if (movementDot < 0) {
                float reversalStrength = -movementDot / (currentMagnitude * prevMagnitude + 0.0001f);
                nodeDamping[i] = std::min(0.95f, nodeDamping[i] + reversalStrength * 0);

                if (reversalStrength > 0.7f && currentMagnitude > 0.5f) {
                    writeG.nodes.positions[i] = writeG.nodes.positions[i] * 0.7f + prevPositions[i] * 0.3f;
                    writeG.nodes.velocities[i] *= 0.5f;
                }
            } else {
                nodeDamping[i] = std::max(0.5f, nodeDamping[i] - 0.01f);
            }
        }

        prevMovements[i] = currentMovement;
        prevPositions[i] = writeG.nodes.positions[i];

        std::vector<uint32_t> neighbors = writeG.GetNeighboursIdx(i);
        int neighborCount = neighbors.size();
        glm::vec3 totalAttractionForce(0, 0, 0);

        for (uint32_t neighborIdx : neighbors) {
            glm::vec3 delta = writeG.nodes.positions[neighborIdx] - writeG.nodes.positions[i];
            float distance = glm::length(delta);

            if (distance < 0.1f)
                continue;

            float connectionFactor = 1.0f / std::sqrt(std::max(1, neighborCount));
            float attractionForce =
                effectiveAttractionStrength * connectionFactor * (distance - parameters.targetDistance);

            if (distance > 1e-10f) {
                glm::vec3 attractForce = glm::normalize(delta) * attractionForce;
                writeG.nodes.forces[i] += attractForce;
                totalAttractionForce += attractForce;
            }
        }

        // Centering force
        float centeringScale = 1.0f / (1.0f + neighborCount * 0.2f);
        glm::vec3 center = glm::vec3(0, 0, 0);
        glm::vec3 toCenter = center - writeG.nodes.positions[i];
        float distanceToCenter = glm::length(toCenter);

        if (distanceToCenter > 50.0f) {
            float centeringStrength = effectiveCenteringForce * centeringScale * (distanceToCenter - 50.0f) / 50.0f;
            writeG.nodes.forces[i] += glm::normalize(toCenter) * centeringStrength;
        }

        writeG.nodes.forces[i] *= parameters.forceMultiplier;

        // Force limiting
        float totalAttractionMagnitude = glm::length(totalAttractionForce);
        float forceMagnitude = glm::length(writeG.nodes.forces[i]);

        if (totalAttractionMagnitude > forceMagnitude * 0.8f && totalAttractionMagnitude > 0.5f) {
            float reductionFactor = (forceMagnitude * 0.8f) / totalAttractionMagnitude;
            writeG.nodes.forces[i] -= totalAttractionForce * (1.0f - reductionFactor);
        }

        forceMagnitude = glm::length(writeG.nodes.forces[i]);
        if (forceMagnitude > parameters.maxForce) {
            writeG.nodes.forces[i] = (writeG.nodes.forces[i] / forceMagnitude) * parameters.maxForce;
        }

        if (std::isnan(writeG.nodes.forces[i].x) || std::isnan(writeG.nodes.forces[i].y) ||
            std::isnan(writeG.nodes.forces[i].z)) {
            writeG.nodes.forces[i] = glm::vec3(0, 0, 0);
        }
    }

    static std::vector<glm::vec3> prevVelocities;
    if (prevVelocities.size() != nodeCount) {
        prevVelocities.resize(nodeCount, glm::vec3(0));
    }

#pragma omp simd
    for (size_t i = 0; i < nodeCount; ++i) {
        if (static_cast<int32_t>(i) == draggingNode.id) {
            writeG.nodes.velocities[i] = glm::vec3(0, 0, 0);
            writeG.nodes.positions[i] = draggingNode.position;
            continue;
        }

        float nodeDampingFactor = nodeDamping[i];
        glm::vec3 acceleration = writeG.nodes.forces[i] / std::max(0.1f, writeG.nodes.masses[i]);

        writeG.nodes.velocities[i] = writeG.nodes.velocities[i] * nodeDampingFactor + acceleration * safeTimeStep;

        if (nodeDampingFactor > 0.8f) {
            writeG.nodes.velocities[i] = writeG.nodes.velocities[i] * 0.6f + prevVelocities[i] * 0.4f;
            prevVelocities[i] = writeG.nodes.velocities[i];
        }

        float velocityMagnitude = glm::length(writeG.nodes.velocities[i]);
        if (velocityMagnitude > parameters.maxVelocity) {
            writeG.nodes.velocities[i] = (writeG.nodes.velocities[i] / velocityMagnitude) * parameters.maxVelocity;
        }

        if (std::isnan(writeG.nodes.velocities[i].x) || std::isnan(writeG.nodes.velocities[i].y) ||
            std::isnan(writeG.nodes.velocities[i].z)) {
            writeG.nodes.velocities[i] = glm::vec3(0, 0, 0);
        }

        glm::vec3 oldPos = writeG.nodes.positions[i];
        writeG.nodes.positions[i] += writeG.nodes.velocities[i] * safeTimeStep;

        // Position change limiting
        float positionChange = glm::length(writeG.nodes.positions[i] - oldPos);
        float maxPositionChange = 2.0f;
        if (positionChange > maxPositionChange) {
            writeG.nodes.positions[i] =
                oldPos + ((writeG.nodes.positions[i] - oldPos) / positionChange) * maxPositionChange;
            writeG.nodes.velocities[i] *= 0.7f;
        }
    }
}

// ------------------------------------------------------------------------------

std::string toLower(const std::string &input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

void GraphEngine::updateGraphPositions(GS::Graph &writeG, const float dt, const SimulationControlData &simControlData) {
    static std::vector<glm::vec3> prevPositions;
    static std::vector<glm::vec3> prevMovements;
    static std::vector<float> nodeDamping;

    const auto parameters = simControlData.parameters.load(std::memory_order_relaxed);
    const auto draggingNode = simControlData.draggingNode.load(std::memory_order_relaxed);

    if (prevPositions.size() != writeG.nodes.positions.size()) {
        prevPositions.resize(writeG.nodes.positions.size());
        prevMovements.resize(writeG.nodes.positions.size(), glm::vec3(0));
        nodeDamping.resize(writeG.nodes.positions.size(), 0.7f);

        prevPositions = writeG.nodes.positions;
    }

    // Reset forces
    std::fill(writeG.nodes.forces.begin(), writeG.nodes.forces.end(), glm::vec3(0, 0, 0));

    static float coolingFactor = 1.0f;
    // coolingFactor = std::max(0.0f, coolingFactor * 0.999f);

    float nodeCountScaling = 1.0f / (1.0f + std::log10(std::max(1.0f, float(writeG.nodes.positions.size()))));

    float effectiveRepulsionStrength = parameters.repulsionStrength * nodeCountScaling * coolingFactor;
    float effectiveAttractionStrength = parameters.attractionStrength * coolingFactor;
    float effectiveCenteringForce = parameters.centeringForce * 0.1f;

    float safeTimeStep = dt * parameters.timeStep;

    for (size_t i = 0; i < writeG.nodes.positions.size(); i++) {
        if (static_cast<int32_t>(i) == draggingNode.id)
            continue;

        glm::vec3 currentMovement = writeG.nodes.positions[i] - prevPositions[i];

        float movementDot = glm::dot(currentMovement, prevMovements[i]);
        float currentMagnitude = glm::length(currentMovement);
        float prevMagnitude = glm::length(prevMovements[i]);

        if (currentMagnitude > 0.01f && prevMagnitude > 0.01f) {
            if (movementDot < 0) {
                float reversalStrength = -movementDot / (currentMagnitude * prevMagnitude + 0.0001f);

                nodeDamping[i] = std::min(0.95f, nodeDamping[i] + reversalStrength * 0);

                if (reversalStrength > 0.7f && currentMagnitude > 0.5f) {
                    writeG.nodes.positions[i] = writeG.nodes.positions[i] * 0.7f + prevPositions[i] * 0.3f;
                    writeG.nodes.velocities[i] *= 0.5f; // Reduce velocity
                }
            } else {
                nodeDamping[i] = std::max(0.5f, nodeDamping[i] - 0.01f);
            }
        }

        prevMovements[i] = currentMovement;
        prevPositions[i] = writeG.nodes.positions[i];
    }

    for (size_t i = 0; i < writeG.nodes.positions.size(); i++) {
        if (static_cast<int32_t>(i) == draggingNode.id)
            continue;

        for (size_t j = 0; j < writeG.nodes.positions.size(); j++) {
            if (i == j)
                continue;

            glm::vec3 delta = writeG.nodes.positions[i] - writeG.nodes.positions[j];
            float distSq = glm::dot(delta, delta);

            const float safeMinDistance = 0.1f;
            if (distSq < safeMinDistance * safeMinDistance) {
                distSq = safeMinDistance * safeMinDistance;
            }

            float repulsionForce = effectiveRepulsionStrength * writeG.nodes.sizes[i] * writeG.nodes.sizes[j] / distSq;

            if (glm::dot(delta, delta) > 1e-10f) {
                writeG.nodes.forces[i] += glm::normalize(delta) * repulsionForce;
            }
        }

        std::vector<uint32_t> neighbors = writeG.GetNeighboursIdx(i);
        int neighborCount = neighbors.size();
        glm::vec3 totalAttractionForce(0, 0, 0);

        for (uint32_t neighborIdx : neighbors) {
            glm::vec3 delta = writeG.nodes.positions[neighborIdx] - writeG.nodes.positions[i];
            float distance = glm::length(delta);

            if (distance < 0.1f)
                continue;

            float connectionFactor = 1.0f / std::sqrt(std::max(1, neighborCount));

            float attractionForce =
                effectiveAttractionStrength * connectionFactor * (distance - parameters.targetDistance);

            if (distance > 1e-10f) {
                glm::vec3 attractForce = glm::normalize(delta) * attractionForce;
                writeG.nodes.forces[i] += attractForce;
                totalAttractionForce += attractForce;
            }
        }

        float centeringScale = 1.0f / (1.0f + neighborCount * 0.2f);

        glm::vec3 center = glm::vec3(0, 0, 0); // Use origin as center instead of graph center
        glm::vec3 toCenter = center - writeG.nodes.positions[i];
        float distanceToCenter = glm::length(toCenter);

        if (distanceToCenter > 50.0f) {
            float centeringStrength = effectiveCenteringForce * centeringScale * (distanceToCenter - 50.0f) / 50.0f;
            writeG.nodes.forces[i] += glm::normalize(toCenter) * centeringStrength;
        }

        writeG.nodes.forces[i] *= parameters.forceMultiplier;

        float totalAttractionMagnitude = glm::length(totalAttractionForce);
        float forceMagnitude = glm::length(writeG.nodes.forces[i]);

        if (totalAttractionMagnitude > forceMagnitude * 0.8f && totalAttractionMagnitude > 0.5f) {
            float reductionFactor = (forceMagnitude * 0.8f) / totalAttractionMagnitude;
            writeG.nodes.forces[i] -= totalAttractionForce * (1.0f - reductionFactor);
        }

        forceMagnitude = glm::length(writeG.nodes.forces[i]);
        if (forceMagnitude > parameters.maxForce) {
            writeG.nodes.forces[i] = (writeG.nodes.forces[i] / forceMagnitude) * parameters.maxForce;
        }

        if (std::isnan(writeG.nodes.forces[i].x) || std::isnan(writeG.nodes.forces[i].y) ||
            std::isnan(writeG.nodes.forces[i].z)) {
            writeG.nodes.forces[i] = glm::vec3(0, 0, 0); // Reset NaN forces
        }
    }

    for (size_t i = 0; i < writeG.nodes.positions.size(); i++) {
        if (static_cast<int32_t>(i) == draggingNode.id) {
            writeG.nodes.velocities[i] = glm::vec3(0, 0, 0);
            writeG.nodes.positions[i] = draggingNode.position;
            continue;
        }

        float nodeDampingFactor = nodeDamping[i];

        glm::vec3 acceleration = writeG.nodes.forces[i] / std::max(0.1f, writeG.nodes.masses[i]);

        writeG.nodes.velocities[i] = writeG.nodes.velocities[i] * nodeDampingFactor + acceleration * safeTimeStep;

        if (nodeDampingFactor > 0.8f) { // Node is likely oscillating
            // Blend new velocity with average of old velocities for smoother transitions
            static std::vector<glm::vec3> prevVelocities;
            if (prevVelocities.size() != writeG.nodes.positions.size()) {
                prevVelocities.resize(writeG.nodes.positions.size(), glm::vec3(0));
            }

            writeG.nodes.velocities[i] = writeG.nodes.velocities[i] * 0.6f + prevVelocities[i] * 0.4f;
            prevVelocities[i] = writeG.nodes.velocities[i];
        }

        float velocityMagnitude = glm::length(writeG.nodes.velocities[i]);
        if (velocityMagnitude > parameters.maxVelocity) {
            writeG.nodes.velocities[i] = (writeG.nodes.velocities[i] / velocityMagnitude) * parameters.maxVelocity;
        }

        if (std::isnan(writeG.nodes.velocities[i].x) || std::isnan(writeG.nodes.velocities[i].y) ||
            std::isnan(writeG.nodes.velocities[i].z)) {
            writeG.nodes.velocities[i] = glm::vec3(0, 0, 0);
        }

        glm::vec3 oldPos = writeG.nodes.positions[i];

        writeG.nodes.positions[i] += writeG.nodes.velocities[i] * safeTimeStep;

        // Position change limiting - prevent large jumps
        float positionChange = glm::length(writeG.nodes.positions[i] - oldPos);
        float maxPositionChange = 2.0f;
        if (positionChange > maxPositionChange) {
            writeG.nodes.positions[i] =
                oldPos + ((writeG.nodes.positions[i] - oldPos) / positionChange) * maxPositionChange;
            writeG.nodes.velocities[i] *= 0.7f;
        }

        // adaptive boundary handling - soft boundaries that push back
        // const float boundarySize = 200.f;
        // for (int d = 0; d < 3; d++) {
        //     if (std::isnan(writeG.nodes.positions[i][d])) {
        //         writeG.nodes.positions[i][d] = 0;
        //         writeG.nodes.velocities[i][d] = 0;
        //     } else if (writeG.nodes.positions[i][d] > boundarySize) {
        //         float excess = writeG.nodes.positions[i][d] - boundarySize;
        //         writeG.nodes.positions[i][d] = boundarySize - 5.0f * (1.0f - std::exp(-excess * 0.1f));
        //         writeG.nodes.velocities[i][d] *= -0.5f;
        //     } else if (writeG.nodes.positions[i][d] < -boundarySize) {
        //         float excess = -boundarySize - writeG.nodes.positions[i][d];
        //         writeG.nodes.positions[i][d] = -boundarySize + 5.0f * (1.0f - std::exp(-excess * 0.1f));
        //         writeG.nodes.velocities[i][d] *= -0.5f;
        //     }
        // }
    }
}

void GraphEngine::processControls(GS::Graph *readGraph, GS::Graph *writeGraph, SimulationControlData &dat) {
    if (m_controlData.graph.searching.load(std::memory_order_relaxed)) {
        globalLogger->info("Loading data for " + m_controlData.graph.searchString);
        writeGraph->Clear();
        search(*writeGraph, m_controlData.graph.searchString);
        setupGraph(*writeGraph, false);
        m_graphBuf.PublishAll();
        globalLogger->info("Published graph data");
        readGraph = m_graphBuf.GetCurrent();
        writeGraph = m_graphBuf.GetWriteBuffer();
        m_controlData.graph.searching.store(false, std::memory_order_relaxed);
        m_controlData.engine.initGraphData.store(true, std::memory_order_relaxed);
    }

    if (m_pendingExpansion.has_value()) {
        auto &expansion = m_pendingExpansion.value();

        if (expansion.m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            try {
                auto linkedPages = expansion.m_future.get();

                int i = 0;
                for (const auto &page : linkedPages) {
                    i++;
                    globalLogger->info("Page" + page.title);
                    const uint32_t idx = writeGraph->AddNode(page.title);
                    writeGraph->AddEdge(expansion.m_sourceNodeId, idx);
                    // writeGraph->nodes.sizes[expansion.m_sourceNodeId]++;
                    // if (i > 25) {
                    //     break;
                    // }
                }

                m_controlData.engine.initGraphData.store(true, std::memory_order_relaxed);
                m_graphBuf.PublishAll();
                readGraph = m_graphBuf.GetCurrent();
                writeGraph = m_graphBuf.GetWriteBuffer();
                dat.resetSimulation = true;

                globalLogger->info("Completed async node expansion for: " + expansion.m_nodeName);
            } catch (const std::exception &e) {
                globalLogger->error("Failed to expand node: " + std::string(e.what()));
            }

            m_pendingExpansion.reset();
        }
    }

    int32_t sourceNode = m_controlData.graph.sourceNode.load(std::memory_order_relaxed);
    if (sourceNode >= 0) {
        m_controlData.graph.sourceNode.store(-1, std::memory_order_relaxed);

        if (!m_pendingExpansion.has_value()) {
            std::string nodeName = readGraph->nodes.titles.at(sourceNode);

            auto future = std::async(std::launch::async, [nodeName, this]() -> std::vector<LinkedPage> {
                std::lock_guard<std::mutex> lock(m_dBInterfaceMutex);
                return m_dB->GetLinkedPages(toLower(nodeName));
            });

            m_pendingExpansion = PendingNodeExpansion{std::move(future), static_cast<uint32_t>(sourceNode), nodeName};

            globalLogger->info("Started async node expansion for: " + nodeName);
        } else {
            globalLogger->info("Node expansion already in progress, ignoring request");
        }
    }

    // if (dat.resetSimulation) {
    //     setupGraph(*writeGraph, false);
    //     graphBuf.Publish();
    //     readGraph = graphBuf.GetCurrent();
    //     writeGraph = graphBuf.GetWriteBuffer();
    //     dat.resetSimulation = false;
    //     controlData.sim.store(dat, std::memory_order_relaxed);
    //     globalLogger->info("Reset simulation");
    // }
}

void GraphEngine::graphPositionSimulation() {
    globalLogger->info("Physics thread starting");

    const auto simulationInterval = std::chrono::milliseconds(0);

    auto frameStart = std::chrono::system_clock::now();

    while (!m_shouldTerminate) {
        GS::Graph *readGraph = m_graphBuf.GetCurrent();
        GS::Graph *writeGraph = m_graphBuf.GetWriteBuffer();

        auto frameEnd = std::chrono::system_clock::now();
        double elapsed_seconds = std::chrono::duration<double>(frameEnd - frameStart).count();
        frameStart = frameEnd;

        processControls(readGraph, writeGraph, m_controlData.sim);
        *writeGraph = *readGraph;
        updateGraphPositionsBarnesHut(*writeGraph, elapsed_seconds, m_controlData.sim);
        m_graphBuf.Publish();

        std::this_thread::sleep_for(simulationInterval);
    }
}

void GraphEngine::generateRealData(GS::Graph &graph) {

    graph.LoadBinary("physics.wiki"); // Use local data for demo
    return;

    std::vector<LinkedPage> linkedPages;

    std::lock_guard<std::mutex> lock(m_dBInterfaceMutex);

    linkedPages = m_dB->GetLinkedPages("physics");

    auto x = graph.AddNode("Physics");

    int i = 0;
    for (const auto &page : linkedPages) {
        i++;
        const uint32_t idx = graph.AddNode(page.title);
        graph.AddEdge(x, idx);
        // graph.nodes.sizes[x]++;
        if (i > 30) {
            break;
        }
    }

    linkedPages = m_dB->GetLinkedPages("multiverse");

    i = 0;
    for (const auto &page : linkedPages) {
        i++;
        const uint32_t idx = graph.AddNode(page.title);
        graph.AddEdge(5, idx);
        // graph.nodes.sizes[5]++;

        if (i > 20) {
            break;
        }
    }

    i = 0;

    linkedPages = m_dB->GetLinkedPages("atom");

    for (const auto &page : linkedPages) {
        i++;
        const uint32_t idx = graph.AddNode(page.title);
        graph.AddEdge(11, idx);
        // graph.nodes.sizes[5]++;

        if (i > 20) {
            break;
        }
    }

    graph.AddEdge(3, 14);
    graph.AddEdge(7, 3);
    graph.AddEdge(8, 11);

    // graph.Clear();
    // graph.LoadBinary("data2.wiki"); // Use local data for demo
    graph.SaveBinary("data2.wiki");
}

void GraphEngine::search(GS::Graph &graph, std::string query) {
    std::vector<LinkedPage> linkedPages;

    std::lock_guard<std::mutex> lock(m_dBInterfaceMutex);
    linkedPages = m_dB->GetLinkedPages(query);

    auto x = graph.AddNode(query.c_str());

    for (const auto &page : linkedPages) {
        const uint32_t idx = graph.AddNode(page.title.c_str());
        graph.AddEdge(x, idx);
        graph.nodes.sizes[x]++;
    }

    globalLogger->info("Search query: ", query, " Number of connected nodes: ", graph.nodes.titles.size());
}

void GraphEngine::setupGraph(GS::Graph &db, bool genData) {
    if (genData) {
        generateRealData(db);
    }
    db.GenerateDefaultData();

    const uint32_t numOfElements = db.nodes.titles.size();

    uint32_t baseNodeIdx = db.GetTopNode();
    auto neighboursUID = db.GetNeighboursIdx(baseNodeIdx);
    auto out = spreadRand(numOfElements, 50.0f);

    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_real_distribution<> dist{0, 1};
    for (uint32_t i = 0; i < numOfElements; i++) {
        auto col = hsv2rgb(dist(gen), 0.8f, 1.0f);
        db.nodes.positions[i] = out[i];
        db.nodes.colors[i] = GS::Color{static_cast<unsigned char>(col.r), static_cast<unsigned char>(col.g),
                                       static_cast<unsigned char>(col.b)};
        db.nodes.sizes[i] = std::sqrt(db.nodes.sizes[i]) * 5;
        db.nodes.edgeSizes[i] = db.nodes.sizes[i] * 0.7f;
    }
}
