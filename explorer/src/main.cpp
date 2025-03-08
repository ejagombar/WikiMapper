#include "../lib/rgb_hsv.hpp"
#include "./visual/engine.hpp"
#include "graph.hpp"
#include "pointMaths.hpp"
#include "store.hpp"
#include <cmath>
#include <cstdint>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <json/json.h>
#include <random>
#include <thread>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/std_image.h"

GS::GraphTripleBuf graphBuf;

float packRGBToFloat(unsigned char r, unsigned char g, unsigned char b) {
    uint32_t packed = (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | (static_cast<uint32_t>(b));
    return *reinterpret_cast<float *>(&packed);
}

void unpackFloatToRGB(float packedFloat, unsigned char &r, unsigned char &g, unsigned char &b) {
    uint32_t packed = *reinterpret_cast<uint32_t *>(&packedFloat);

    r = (packed >> 16) & 0xFF;
    g = (packed >> 8) & 0xFF;
    b = packed & 0xFF;
}

void generateRealData(GS::Graph &graph) {
    Neo4jInterface neo4jDB("http://127.0.0.1:7474");
    if (!neo4jDB.Authenticate("neo4j", "test1234")) {
        return;
    }

    auto randomPage = neo4jDB.GetRandomPages(1).at(0);
    auto linkedPages = neo4jDB.GetLinkedPages("mathematics");

    graph.AddNode("Mathematics");

    for (const auto &page : linkedPages) {
        const uint32_t idx = graph.AddNode(page.title.c_str());
        graph.AddEdge(0, idx);
    }
}

void updateGraphPositionsShit(GS::Graph &readG, GS::Graph &writeG) {
    // Define the layout area and compute an ideal distance between nodes.
    // In many algorithms, k is chosen as the square root of (area / number_of_nodes).
    const float area = 10000.0f; // e.g., a 100x100 space.
    const size_t nodeCount = readG.nodes.size();

    const float k = std::sqrt(area / static_cast<float>(nodeCount));

    // Temperature (max displacement per iteration) starts high and cools down gradually.
    static float temperature = 0.9f;
    const float coolingFactor = 0.98f; // Cooling multiplier per iteration.

    // Initialize displacement for each node.
    std::vector<glm::vec3> disp(nodeCount, glm::vec3(0.0f));

    // --- Compute repulsive forces (displacements) between all pairs of nodes ---
    // for (size_t i = 0; i < nodeCount; i++) {
    //     for (size_t j = i + 1; j < nodeCount; j++) {
    //         glm::vec3 delta = readG.nodes[i].pos - readG.nodes[j].pos;
    //         float distance = glm::length(delta);
    //         // Prevent division by zero and ensure a minimum distance.
    //         if (distance < 0.01f)
    //             distance = 0.01f;
    //         glm::vec3 direction = delta / distance;
    //         // Repulsive force magnitude: (k^2 / distance)
    //         float repulsiveForce = ((k * k) / distance) / 100;
    //         glm::vec3 forceVec = direction * repulsiveForce;
    //         disp[i] += forceVec;
    //         disp[j] -= forceVec;
    //     }
    // }

    // --- Compute attractive forces (displacements) along edges ---
    for (const GS::Edge &edge : readG.edges) {
        uint32_t i = edge.startIdx;
        uint32_t j = edge.endIdx;
        glm::vec3 delta = readG.nodes[i].pos - readG.nodes[j].pos;
        float distance = glm::length(delta);
        if (distance < 0.01f)
            distance = 0.01f;
        glm::vec3 direction = delta / distance;
        // Attractive force magnitude: (distance^2 / k)
        float attractiveForce = (distance * distance) / k;
        glm::vec3 forceVec = direction * attractiveForce;
        disp[i] -= forceVec;
        disp[j] += forceVec;
    }

    // --- Update positions by applying the computed displacements ---
    for (size_t i = 0; i < nodeCount; i++) {
        glm::vec3 displacement = disp[i];
        float dispLength = glm::length(displacement);
        if (dispLength < 0.01f)
            dispLength = 0.01f;
        // Limit the maximum displacement to the current temperature value.
        glm::vec3 limitedDisp = (displacement / dispLength) * std::min(dispLength, temperature);
        glm::vec3 newPos = readG.nodes[i].pos + limitedDisp;

        // Update the output node. Here we copy the previous state and then adjust.
        writeG.nodes[i] = readG.nodes[i];
        writeG.nodes[i].pos = newPos;
        // Optionally, set the new velocity as the displacement (for visualization or further use).
        writeG.nodes[i].vel = limitedDisp;
        // Store the net force (displacement) for debugging or visualization.
        writeG.nodes[i].force = disp[i];
    }
    writeG.nodes[1].pos = readG.nodes[1].pos + glm::vec3(0, 0.01, 0);
    writeG.nodes[100].pos = readG.nodes[100].pos + glm::vec3(0, -0.01, 0);

    // --- Cool down the system ---
    temperature *= coolingFactor;
}

void updateGraphPositions(const GS::Graph &readG, GS::Graph &writeG, const float dt) {
    const float qqMultiplier = 0.0001f;
    const float gravityMultiplier = 1.f;
    const float accelSizeMultiplier = 1.0f;

    const float area = 10000.0f;

    const int nodeCount = readG.nodes.size();

    const float k = 1 / std::sqrt(area / static_cast<float>(nodeCount));

    std::vector<glm::vec3> nodeForces;
    nodeForces.resize(nodeCount);

    // Apply gravity towards (0,0,0)
    for (uint i = 0; i < nodeCount; i++) {
        nodeForces[i] = readG.nodes[i].force;
        std::cout << nodeForces[i].x + nodeForces[i].y + nodeForces[i].z << std::endl;
        if (glm::length(readG.nodes[i].pos) != 0) [[likely]] {
            nodeForces[i] -= glm::normalize(readG.nodes[i].pos) * gravityMultiplier;
        }
    }

    // Apply node-node repulsion using coulomb's force
    for (uint i = 0; i < nodeCount; i++) {
        const GS::Node &node1 = readG.nodes[i];

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
    }

    // for (const GS::Edge &edge : readG.edges) {
    //     glm::vec3 delta = readG.nodes[edge.startIdx].pos - readG.nodes[edge.endIdx].pos;
    //     float distance = glm::length(delta);
    //
    //     if (distance == 0) {
    //         continue;
    //     }
    //
    //     if (distance < 0.01f) {
    //         distance = 0.01f;
    //     }
    //
    //     const glm::vec3 direction = delta / distance;
    //
    //     const float attractiveForce = distance * distance * 0.01;
    //     const glm::vec3 forceVec = direction * attractiveForce;
    //
    //     nodeForces[edge.endIdx] += forceVec;
    //     // nodeForces[edge.startIdx] = forceVec;
    // }

    // Apply forces and update velocity, position
    for (uint i = 0; i < nodeCount; i++) {

        // if (glm::length(nodeForces[i]) < 0.2) {
        //     nodeForces[i] = glm::vec3(0);
        // }

        const glm::vec3 acceleration = (nodeForces[i] * (1.0f / readG.nodes[i].size)) * accelSizeMultiplier;
        const glm::vec3 vel = readG.nodes[i].vel + acceleration * dt;

        writeG.nodes[i].force = nodeForces[i];
        writeG.nodes[i].vel = vel;
        writeG.nodes[i].pos = readG.nodes[i].pos + vel * dt;
    }
}

void graphPositionSimulation() {
    const auto simulationInterval = std::chrono::milliseconds(10);
    while (true) {
        GS::Graph *readGraph = graphBuf.GetCurrent();
        GS::Graph *writeGraph = graphBuf.GetWriteBuffer();

        updateGraphPositions(*readGraph, *writeGraph, 0.1);
        graphBuf.Publish();

        std::this_thread::sleep_for(simulationInterval);
    }
}

void setupGraph(GS::Graph &db) {
    generateRealData(db);

    const int numOfElements = db.nodes.size();

    uint32_t baseNodeIdx = db.GetTopNode();
    auto baseNode = db.nodes[baseNodeIdx];
    std::cout << baseNode.title;

    auto neighboursUID = db.GetNeighboursIdx(baseNodeIdx);
    auto out = spreadOrbit(glm::vec3(0), neighboursUID.size(), 2 * sqrt(numOfElements), glm::vec3(0, 0, 0));

    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_real_distribution<> dist{0, 1};
    for (int i = 0; i <= neighboursUID.size(); i++) {
        auto col = hsv2rgb(dist(gen), 0.8f, 1.0f);
        db.nodes[i].pos = out[i];
        db.nodes[i].rgb[0] = static_cast<char>(col.r);
        db.nodes[i].rgb[1] = static_cast<char>(col.g);
        db.nodes[i].rgb[2] = static_cast<char>(col.b);
        db.nodes[i].size = 20;
        db.nodes[i].edgeSize = 5;
    }

    db.nodes[baseNodeIdx].pos = glm::vec3(0);
}

void test(int c) { std::cout << c; }

int main() {
    GS::Graph *writeGraph = graphBuf.GetWriteBuffer();
    setupGraph(*writeGraph);
    graphBuf.Publish();

    GS::Graph *readgraph = graphBuf.GetCurrent();
    writeGraph = graphBuf.GetWriteBuffer();

    *writeGraph = *readgraph;
    graphBuf.Publish();

    readgraph = graphBuf.GetCurrent();
    writeGraph = graphBuf.GetWriteBuffer();

    *writeGraph = *readgraph;
    graphBuf.Publish();

    // Engine renderEngine(graphBuf);
    //
    std::thread t{graphPositionSimulation};
    t.join();

    // renderEngine.Run();
    std::cout << "Ended" << std::endl;
}
