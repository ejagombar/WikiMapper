#include "../lib/rgb_hsv.hpp"
#include "controlData.hpp"
#include "graph.hpp"
#include "pointMaths.hpp"
#include "simulation.hpp"
#include "store.hpp"
#include "visual/engine.hpp"
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <json/json.h>
#include <random>
#include <thread>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/std_image.h"

GS::GraphTripleBuf graphBuf;
ControlData controlData;

void generateRealData(GS::Graph &graph) {
    Neo4jInterface neo4jDB("http://127.0.0.1:7474");
    if (!neo4jDB.Authenticate("neo4j", "test1234")) {
        return;
    }

    // auto randomPage = neo4jDB.GetRandomPages(1).at(0);
    auto linkedPages = neo4jDB.GetLinkedPages("physics");

    auto x = graph.AddNode("Physics");

    for (const auto &page : linkedPages) {
        // std::cout << page.title << std::endl;
        const uint32_t idx = graph.AddNode(page.title.c_str());
        graph.AddEdge(x, idx);
        graph.nodes[x].size++;
    }

    std::cout << "Query: " << "Physics" << " Size: " << graph.nodes.size() << std::endl;
}

void search(GS::Graph &graph, std::string query) {
    Neo4jInterface neo4jDB("http://127.0.0.1:7474");
    if (!neo4jDB.Authenticate("neo4j", "test1234")) {
        return;
    }

    // auto randomPage = neo4jDB.GetRandomPages(1).at(0);
    auto linkedPages = neo4jDB.GetLinkedPages(query);

    // auto x = graph.AddNode("Nico Ditch");
    // std::cout << "Size: " << linkedPages.size() << " Start IDX: " << x << std::endl;

    for (const auto &page : linkedPages) {
        // std::cout << page.title << std::endl;
        const uint32_t idx = graph.AddNode(page.title.c_str());
        graph.AddEdge(0, idx);
        graph.nodes[0].size++;
    }

    std::cout << "Query: " << query << " Size: " << graph.nodes.size() << std::endl;
}

void setupGraph(GS::Graph &db, bool genData = true) {
    if (genData) {
        generateRealData(db);
    }

    const uint32_t numOfElements = db.nodes.size();

    uint32_t baseNodeIdx = db.GetTopNode();
    auto baseNode = db.nodes[baseNodeIdx];
    // std::cout << baseNode.title;

    auto neighboursUID = db.GetNeighboursIdx(baseNodeIdx);
    auto out = spreadRand(numOfElements, 50.0f);

    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_real_distribution<> dist{0, 1};
    for (uint32_t i = 0; i < numOfElements; i++) {
        auto col = hsv2rgb(dist(gen), 0.8f, 1.0f);
        db.nodes[i].pos = out[i];
        db.nodes[i].rgb[0] = static_cast<char>(col.r);
        db.nodes[i].rgb[1] = static_cast<char>(col.g);
        db.nodes[i].rgb[2] = static_cast<char>(col.b);
        db.nodes[i].size = 20;
        db.nodes[i].edgeSize = 5;
    }
}

void graphPositionSimulation() {
    const auto simulationInterval = std::chrono::milliseconds(200);

    auto simStart = std::chrono::system_clock::now();
    auto frameStart = simStart;

    while (true) {
        GS::Graph *readGraph = graphBuf.GetCurrent();
        GS::Graph *writeGraph = graphBuf.GetWriteBuffer();

        auto frameEnd = std::chrono::system_clock::now();
        float elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
        frameStart = frameEnd;

        SimulationControlData dat = controlData.sim.load(std::memory_order_relaxed);
        // if (controlData.graph.searching.load(std::memory_order_relaxed)) {
        //     std::cout << controlData.graph.searchString << std::endl;
        //     controlData.graph.searching.store(false);
        // }

        if (controlData.graph.searching.load(std::memory_order_relaxed)) {
            std::cout << "Loading data for " + controlData.graph.searchString + "." << std::endl;
            writeGraph->Clear();
            search(*writeGraph, controlData.graph.searchString);
            setupGraph(*writeGraph, false);

            graphBuf.PublishAll();
            std::cout << "PUblished graph data" << std::endl;

            readGraph = graphBuf.GetCurrent();
            writeGraph = graphBuf.GetWriteBuffer();

            controlData.graph.searching.store(false, std::memory_order_relaxed);
            controlData.engine.initGraphData.store(true, std::memory_order_relaxed);
            simStart = std::chrono::system_clock::now();
        }

        if (dat.resetSimulation) {
            simStart = std::chrono::system_clock::now();
        }

        dat.forceMultiplier = 1.f;
        if (std::chrono::duration_cast<std::chrono::seconds>(frameEnd - simStart).count() > 4.) {
            dat.forceMultiplier = 0.0f;
        }

        if (dat.resetSimulation) {
            setupGraph(*writeGraph, false);
            graphBuf.Publish();
            readGraph = graphBuf.GetCurrent();
            writeGraph = graphBuf.GetWriteBuffer();
            dat.resetSimulation = false;
            controlData.sim.store(dat, std::memory_order_relaxed);
        }

        // updateGraphPositions(*readGraph, *writeGraph, elapsed_seconds, dat);
        graphBuf.Publish();

        std::this_thread::sleep_for(simulationInterval);
    }
}

int main() {
    GS::Graph *writeGraph = graphBuf.GetWriteBuffer();
    setupGraph(*writeGraph);
    graphBuf.PublishAll();

    Engine renderEngine(graphBuf, controlData);

    std::thread t{graphPositionSimulation};
    t.detach();

    renderEngine.Run();
    std::cout << "Ended" << std::endl;
}
