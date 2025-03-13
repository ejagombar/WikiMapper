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
    auto linkedPages = neo4jDB.GetLinkedPages("motorola");

    std::cout << "Size: " << linkedPages.size() << std::endl;
    graph.AddNode("motorola");

    for (const auto &page : linkedPages) {
        const uint32_t idx = graph.AddNode(page.title.c_str());
        graph.AddEdge(0, idx);
    }
}

void updateGraphPositions(const GS::Graph &readG, GS::Graph &writeG, const float dt) {
    const float qqMultiplier = 0.05f;
    const float gravityMultiplier = 30.f;
    const float accelSizeMultiplier = 0.01f;
    const float targetDistance = 100;

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

void graphPositionSimulation() {
    const auto simulationInterval = std::chrono::milliseconds(1);

    auto start = std::chrono::system_clock::now();

    while (true) {
        GS::Graph *readGraph = graphBuf.GetCurrent();
        GS::Graph *writeGraph = graphBuf.GetWriteBuffer();

        auto end = std::chrono::system_clock::now();
        float elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        start = end;
        updateGraphPositions(*readGraph, *writeGraph, elapsed_seconds);
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
    for (uint i = 0; i <= neighboursUID.size(); i++) {
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

    Engine renderEngine(graphBuf);

    std::thread t{graphPositionSimulation};
    t.detach();

    renderEngine.Run();
    std::cout << "Ended" << std::endl;
}
