#include "../lib/rgb_hsv.hpp"
#include "./visual/engine.hpp"
#include "graph.hpp"
#include "pointMaths.hpp"
#include "store.hpp"
#include <cmath>
#include <cstdint>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
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

void updateGraphPositions(GS::Graph &readG, GS::Graph &writeG) {}

void graphPositionSimulation() {
    const auto simulationInterval = std::chrono::milliseconds(15);
    while (true) {
        GS::Graph *readGraph = graphBuf.GetCurrent();
        GS::Graph *writeGraph = graphBuf.GetWriteBuffer();

        updateGraphPositions(*readGraph, *writeGraph);
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

    Engine renderEngine(graphBuf);

    std::thread t{graphPositionSimulation};

    renderEngine.Run();
    std::cout << "Ended" << std::endl;
}
