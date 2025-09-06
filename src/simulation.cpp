#include "../lib/rgb_hsv.hpp"
#include "pointMaths.hpp"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <future>
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <json/json.h>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include "controlData.hpp"
#include "logger.hpp"

#include "simulation.hpp"

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

    for (size_t i = 1; i < positions.size(); ++i) {
        minBounds.x = std::min(minBounds.x, positions[i].x);
        minBounds.y = std::min(minBounds.y, positions[i].y);
        minBounds.z = std::min(minBounds.z, positions[i].z);

        maxBounds.x = std::max(maxBounds.x, positions[i].x);
        maxBounds.y = std::max(maxBounds.y, positions[i].y);
        maxBounds.z = std::max(maxBounds.z, positions[i].z);
    }

    glm::vec3 margin = (maxBounds - minBounds) * 0.01f + glm::vec3(1.0f);
    minBounds -= margin;
    maxBounds += margin;
}

void BarnesHutTree::build(const std::vector<glm::vec3> &positions, const std::vector<float> &masses) {
    clear();

    if (positions.empty())
        return;

    m_rootIndex = allocateNode();
    Node &root = m_nodes[m_rootIndex];

    computeBounds(positions, root.minBounds, root.maxBounds);

    for (size_t i = 0; i < positions.size(); ++i) {
        insertBody(m_rootIndex, i, positions[i], masses[i], 0);
    }
}

bool BarnesHutTree::useNodeAsSingleBody(const Node &node, const glm::vec3 &bodyPos) const {
    if (node.bodyCount <= 1)
        return true;

    glm::vec3 size = node.maxBounds - node.minBounds;
    float maxSize = std::max({size.x, size.y, size.z});

    glm::vec3 delta = node.centerOfMass - bodyPos;
    float distanceSq = glm::dot(delta, delta);

    float ratio = (maxSize * maxSize) / distanceSq;
    return ratio < (THETA * THETA);
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

    return stats;
}

void BarnesHutTree::verifyTree() const {
    if (m_rootIndex < 0)
        return;

    std::function<float(int32_t)> verifyNode = [&](int32_t nodeIndex) -> float {
        if (nodeIndex < 0)
            return 0.0f;

        const Node &node = m_nodes[nodeIndex];

        if (node.bodyIndex >= 0) {
            if (node.bodyCount != 1) {
                globalLogger->error("Leaf node {} has bodyCount={} (should be 1)", nodeIndex, node.bodyCount);
            }
            return node.totalMass;
        }

        float childMassSum = 0.0f;

        for (int i = 0; i < 8; ++i) {
            if (node.children[i] >= 0) {
                childMassSum += verifyNode(node.children[i]);
            }
        }

        if (std::abs(node.totalMass - childMassSum) > 1e-5f) {
            globalLogger->error("Node {} mass mismatch: has {:.2f}, children sum to {:.2f}", nodeIndex, node.totalMass,
                                childMassSum);
        }

        return node.totalMass;
    };

    globalLogger->info("Verifying Barnes-Hut tree integrity...");
    verifyNode(m_rootIndex);
    globalLogger->info("Tree verification complete");
}

void BarnesHutTree::insertBody(int32_t nodeIndex, int32_t bodyIndex, const glm::vec3 &pos, float mass, int depth) {
    if (depth > MAX_DEPTH)
        return;

    if (m_nodes[nodeIndex].bodyCount == 0) {
        m_nodes[nodeIndex].bodyIndex = bodyIndex;
        m_nodes[nodeIndex].centerOfMass = pos;
        m_nodes[nodeIndex].totalMass = mass;
        m_nodes[nodeIndex].bodyCount = 1;
        return;
    }

    // Handle subdivision
    if (m_nodes[nodeIndex].bodyIndex >= 0) {
        int32_t existingBodyIndex = m_nodes[nodeIndex].bodyIndex;
        glm::vec3 existingPos = m_nodes[nodeIndex].centerOfMass;
        float existingMass = m_nodes[nodeIndex].totalMass;

        m_nodes[nodeIndex].bodyIndex = -1;

        glm::vec3 center = (m_nodes[nodeIndex].minBounds + m_nodes[nodeIndex].maxBounds) * 0.5f;
        int existingOctant = getOctant(existingPos, center);

        if (m_nodes[nodeIndex].children[existingOctant] < 0) {
            m_nodes[nodeIndex].children[existingOctant] = allocateNode();

            // Reaccess after potential reallocation
            for (int i = 0; i < 3; ++i) {
                if (existingOctant & (1 << i)) {
                    m_nodes[m_nodes[nodeIndex].children[existingOctant]].minBounds[i] = center[i];
                    m_nodes[m_nodes[nodeIndex].children[existingOctant]].maxBounds[i] = m_nodes[nodeIndex].maxBounds[i];
                } else {
                    m_nodes[m_nodes[nodeIndex].children[existingOctant]].minBounds[i] = m_nodes[nodeIndex].minBounds[i];
                    m_nodes[m_nodes[nodeIndex].children[existingOctant]].maxBounds[i] = center[i];
                }
            }
        }

        insertBody(m_nodes[nodeIndex].children[existingOctant], existingBodyIndex, existingPos, existingMass,
                   depth + 1);
    }

    glm::vec3 center = (m_nodes[nodeIndex].minBounds + m_nodes[nodeIndex].maxBounds) * 0.5f;
    int octant = getOctant(pos, center);
    if (m_nodes[nodeIndex].children[octant] < 0) {
        m_nodes[nodeIndex].children[octant] = allocateNode();
        Node &child = m_nodes[m_nodes[nodeIndex].children[octant]];

        for (int i = 0; i < 3; ++i) {
            if (octant & (1 << i)) {
                child.minBounds[i] = center[i];
                child.maxBounds[i] = m_nodes[nodeIndex].maxBounds[i];
            } else {
                child.minBounds[i] = m_nodes[nodeIndex].minBounds[i];
                child.maxBounds[i] = center[i];
            }
        }
    }

    insertBody(m_nodes[nodeIndex].children[octant], bodyIndex, pos, mass, depth + 1);

    float newTotalMass = m_nodes[nodeIndex].totalMass + mass;
    if (newTotalMass > 0) {
        m_nodes[nodeIndex].centerOfMass =
            (m_nodes[nodeIndex].centerOfMass * m_nodes[nodeIndex].totalMass + pos * mass) / newTotalMass;
        m_nodes[nodeIndex].totalMass = newTotalMass;
    }
    m_nodes[nodeIndex].bodyCount++;
}

// Fixed force calculation with proper distance handling
glm::vec3 BarnesHutTree::calculateForceRecursive(int32_t nodeIndex, const glm::vec3 &bodyPos, float bodyMass,
                                                 float bodySizeSq, float repulsionStrength) const {
    if (nodeIndex < 0)
        return glm::vec3(0.0f);

    const Node &node = m_nodes[nodeIndex];

    if (node.bodyCount == 0)
        return glm::vec3(0.0f);

    glm::vec3 delta = node.centerOfMass - bodyPos;
    float distSq = glm::dot(delta, delta);

    if (node.bodyCount == 1 && node.bodyIndex >= 0 && distSq < 1e-6f) {
        return glm::vec3(0.0f);
    }

    float minDist = 1.0f;
    distSq = std::max(distSq, minDist * minDist);

    if (useNodeAsSingleBody(node, bodyPos)) {
        float dist = std::sqrt(distSq);
        float force = repulsionStrength * node.totalMass / distSq;

        force = std::min(force, 100.0f);

        return -delta * (force / dist);
    } else {
        glm::vec3 totalForce(0.0f);

        for (int i = 0; i < 8; ++i) {
            if (node.children[i] >= 0) {
                totalForce +=
                    calculateForceRecursive(node.children[i], bodyPos, bodyMass, bodySizeSq, repulsionStrength);
            }
        }

        return totalForce;
    }
}

void updateGraphPositionsBarnesHut(GS::Graph &writeG, float dt, const SimulationControlData &simControlData) {
    static std::vector<glm::vec3> smoothedVelocities;
    static BarnesHutTree bhTree;

    const auto parameters = simControlData.parameters.load(std::memory_order_relaxed);
    const auto draggingNode = simControlData.draggingNode.load(std::memory_order_relaxed);

    size_t nodeCount = writeG.nodes.positions.size();

    if (smoothedVelocities.size() != nodeCount) {
        smoothedVelocities.resize(nodeCount, glm::vec3(0));
    }

    std::fill(writeG.nodes.forces.begin(), writeG.nodes.forces.end(), glm::vec3(0, 0, 0));

    float nodeCountScaling = 1.0f / std::sqrt(std::max(1.0f, float(nodeCount)));

    float effectiveRepulsionStrength = parameters.repulsionStrength * nodeCountScaling * 50.0f;
    float effectiveAttractionStrength = parameters.attractionStrength * 0.1f;
    float effectiveCenteringForce = parameters.centeringForce * 0.01f;

    // Fixed time step
    float safeTimeStep = std::min(dt, 0.016f) * parameters.timeStep;

    bhTree.build(writeG.nodes.positions, writeG.nodes.masses);

    // Calculate repulsion forces using Barnes-Hut
    for (size_t i = 0; i < nodeCount; ++i) {
        if (static_cast<int32_t>(i) == draggingNode.id)
            continue;

        writeG.nodes.forces[i] += bhTree.calculateForce(i, writeG.nodes.positions, writeG.nodes.masses,
                                                        writeG.nodes.sizes, effectiveRepulsionStrength);
    }

    // Calculate attraction forces and apply centering
    for (size_t i = 0; i < nodeCount; ++i) {
        if (static_cast<int32_t>(i) == draggingNode.id)
            continue;

        // Attraction forces from edges
        std::vector<uint32_t> neighbors = writeG.GetNeighboursIdx(i);

        for (uint32_t neighborIdx : neighbors) {
            glm::vec3 delta = writeG.nodes.positions[neighborIdx] - writeG.nodes.positions[i];
            float distance = glm::length(delta);

            if (distance < 0.01f)
                continue;

            // Spring force
            float desiredDistance = parameters.targetDistance * 10.0f;
            float springForce = effectiveAttractionStrength * (distance - desiredDistance);

            // Limit spring force
            springForce = std::max(-10.0f, std::min(10.0f, springForce));

            if (distance > 1e-10f) {
                writeG.nodes.forces[i] += (delta / distance) * springForce;
            }
        }

        // centering force
        glm::vec3 toCenter = -writeG.nodes.positions[i];
        float distanceToCenter = glm::length(toCenter);

        if (distanceToCenter > 10.0f) {
            float centeringStrength = effectiveCenteringForce * std::log(1.0f + distanceToCenter / 10.0f);
            writeG.nodes.forces[i] += (toCenter / distanceToCenter) * centeringStrength;
        }

        writeG.nodes.forces[i] *= parameters.forceMultiplier;

        float forceMagnitude = glm::length(writeG.nodes.forces[i]);
        float maxForce = parameters.maxForce * 10.0f;
        if (forceMagnitude > maxForce) {
            writeG.nodes.forces[i] = (writeG.nodes.forces[i] / forceMagnitude) * maxForce;
        }
    }

    const float globalDamping = 0.85f;
    const float velocitySmoothing = 0.8f;

    std::vector<float> nodeDamping(nodeCount);
    for (size_t i = 0; i < nodeCount; ++i) {
        std::vector<uint32_t> neighbors = writeG.GetNeighboursIdx(i);
        int connectionCount = neighbors.size();

        float connectivityDamping = 0.7f + 0.25f * std::tanh(connectionCount / 5.0f);
        nodeDamping[i] = std::min(0.98f, connectivityDamping);
    }

    for (size_t i = 0; i < nodeCount; ++i) {
        if (static_cast<int32_t>(i) == draggingNode.id) {
            writeG.nodes.velocities[i] = glm::vec3(0, 0, 0);
            writeG.nodes.positions[i] = draggingNode.position;
            smoothedVelocities[i] = glm::vec3(0, 0, 0);
            continue;
        }

        glm::vec3 acceleration = writeG.nodes.forces[i] / std::max(0.1f, writeG.nodes.masses[i]);

        float damping = globalDamping * nodeDamping[i];
        writeG.nodes.velocities[i] = writeG.nodes.velocities[i] * damping + acceleration * safeTimeStep;

        smoothedVelocities[i] =
            smoothedVelocities[i] * (1.0f - velocitySmoothing) + writeG.nodes.velocities[i] * velocitySmoothing;

        float velocityMagnitude = glm::length(smoothedVelocities[i]);
        float maxVel = parameters.maxVelocity * 5.0f;
        if (velocityMagnitude > maxVel) {
            smoothedVelocities[i] = (smoothedVelocities[i] / velocityMagnitude) * maxVel;
        }

        glm::vec3 oldPos = writeG.nodes.positions[i];
        writeG.nodes.positions[i] += smoothedVelocities[i] * safeTimeStep;

        glm::vec3 posChange = writeG.nodes.positions[i] - oldPos;
        float maxPosChange = 5.0f;
        float posChangeMag = glm::length(posChange);
        if (posChangeMag > maxPosChange) {
            writeG.nodes.positions[i] = oldPos + (posChange / posChangeMag) * maxPosChange;
            smoothedVelocities[i] *= 0.5f; // Reduce velocity if position was clamped
        }
    }
}

// ------------------------------------------------------------------------------

std::string toLower(const std::string &input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
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
                    writeGraph->nodes.sizes[expansion.m_sourceNodeId]++;
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
