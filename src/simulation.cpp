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

#ifdef DEBUG_FORCES
    std::cout << "Before update:" << std::endl;
    for (size_t i = 0; i < 3 && i < writeG.nodes.positions.size(); i++) {
        std::cout << "Node " << i << " pos: (" << writeG.nodes[i].pos.x << ", " << writeG.nodes[i].pos.y << ", "
                  << writeG.nodes[i].pos.z << ")" << std::endl;
    }
#endif

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

        std::vector<uint32_t> neighbors = writeG.GetNeighboursIdx(i);
        int neighborCount = neighbors.size();

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
#ifdef DEBUG_FORCES
            std::cout << "NaN force detected for node " << i << std::endl;
#endif
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
        const float boundarySize = 200.f;
        for (int d = 0; d < 3; d++) {
            if (std::isnan(writeG.nodes.positions[i][d])) {
                writeG.nodes.positions[i][d] = 0;
                writeG.nodes.velocities[i][d] = 0;
            } else if (writeG.nodes.positions[i][d] > boundarySize) {
                float excess = writeG.nodes.positions[i][d] - boundarySize;
                writeG.nodes.positions[i][d] = boundarySize - 5.0f * (1.0f - std::exp(-excess * 0.1f));
                writeG.nodes.velocities[i][d] *= -0.5f;
            } else if (writeG.nodes.positions[i][d] < -boundarySize) {
                float excess = -boundarySize - writeG.nodes.positions[i][d];
                writeG.nodes.positions[i][d] = -boundarySize + 5.0f * (1.0f - std::exp(-excess * 0.1f));
                writeG.nodes.velocities[i][d] *= -0.5f;
            }
        }
    }

#ifdef DEBUG_FORCES
    std::cout << "After update:" << std::endl;
    for (size_t i = 0; i < 3 && i < writeG.nodes.positions.size(); i++) {
        std::cout << "Node " << i << " pos: (" << writeG.nodes.positions[i].x << ", " << writeG.nodes.positions[i].y
                  << ", " << writeG.nodes.positions[i].z << ")" << std::endl;
    }
#endif

    // Reset simulation if requested
    if (simControlData.resetSimulation) {

        // std::fill(writeG.nodes.velocities.begin(), writeG.nodes.velocities.end(), glm::vec3(0.0f));
        // std::fill(writeG.nodes.forces.begin(), writeG.nodes.forces.end(), glm::vec3(0.0f));
        // std::fill(writeG.nodes.fixed.begin(), writeG.nodes.fixed.end(), false);
        //
        // std::random_device rd;
        // std::mt19937 gen(rd());
        // std::uniform_real_distribution<> dis(-10.0, 10.0); // Start much closer to center
        //
        // for (size_t i = 0; i < writeG.nodes.positions.size(); ++i) {
        //     writeG.nodes.positions[i] = glm::vec3(dis(gen), dis(gen), dis(gen));
        // }
        //
        // // Reset history data
        // for (size_t i = 0; i < writeG.nodes.positions.size(); i++) {
        //     prevPositions[i] = writeG.nodes.positions[i];
        //     prevMovements[i] = glm::vec3(0);
        //     nodeDamping[i] = 0.7f;
        // }

        // Reset the cooling factor when simulation is reset
        coolingFactor = 1.0f;
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
                    if (i > 25) {
                        break;
                    }
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
        updateGraphPositions(*writeGraph, elapsed_seconds, m_controlData.sim);
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
