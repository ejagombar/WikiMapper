#include "../lib/rgb_hsv.hpp"
#include "controlData.hpp"
#include "graph.hpp"
#include "logger.hpp"
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
#include <json/json.h>
#include <random>
#include <thread>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/std_image.h"

std::shared_ptr<spdlog::logger> globalLogger = spdlog::basic_logger_mt("global", "wikimapper.log");

GS::GraphTripleBuf graphBuf;
ControlData controlData;
std::atomic<bool> shouldTerminate(false);

void generateRealData(GS::Graph &graph) {
    // graph.LoadBinary("../data.wiki"); // Use local data for demo
    // return;

    Neo4jInterface neo4jDB("http://127.0.0.1:7474");
    if (!neo4jDB.Authenticate("neo4j", "test1234")) {
        return;
    }

    // auto randomPage = neo4jDB.GetRandomPages(1).at(0);
    auto linkedPages = neo4jDB.GetLinkedPages("physics");

    auto x = graph.AddNode("Physics");

    int i = 0;
    for (const auto &page : linkedPages) {
        i++;
        const uint32_t idx = graph.AddNode(page.title.c_str());
        graph.AddEdge(x, idx);
        graph.nodes[x].size++;
        if (i > 30) {
            break;
        }
    }

    linkedPages = neo4jDB.GetLinkedPages("multiverse");

    i = 0;
    for (const auto &page : linkedPages) {
        i++;
        const uint32_t idx = graph.AddNode(page.title.c_str());
        graph.AddEdge(5, idx);
        graph.nodes[5].size++;

        if (i > 20) {
            break;
        }
    }

    i = 0;

    linkedPages = neo4jDB.GetLinkedPages("atom");
    for (const auto &page : linkedPages) {
        i++;
        const uint32_t idx = graph.AddNode(page.title.c_str());
        graph.AddEdge(11, idx);
        graph.nodes[11].size++;

        if (i > 20) {
            break;
        }
    }

    graph.AddEdge(3, 14);
    graph.AddEdge(7, 3);
    graph.AddEdge(8, 11);

    // graph.SaveBinary("data.wiki");
}

void search(GS::Graph &graph, std::string query) {
    Neo4jInterface neo4jDB("http://127.0.0.1:7474");
    if (!neo4jDB.Authenticate("neo4j", "test1234")) {
        return;
    }

    auto linkedPages = neo4jDB.GetLinkedPages(query);
    auto x = graph.AddNode(query.c_str());

    for (const auto &page : linkedPages) {
        const uint32_t idx = graph.AddNode(page.title.c_str());
        graph.AddEdge(0, idx);
        graph.nodes[0].size++;
    }

    globalLogger->info("Search query: ", query, " Number of connected nodes: ", graph.nodes.size());
}

void setupGraph(GS::Graph &db, bool genData = true) {
    if (genData) {
        generateRealData(db);
    }

    const uint32_t numOfElements = db.nodes.size();

    uint32_t baseNodeIdx = db.GetTopNode();
    auto baseNode = db.nodes[baseNodeIdx];

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
        db.nodes[i].size = std::sqrt(db.nodes[i].size) * 5;
        db.nodes[i].edgeSize = db.nodes[i].size * 0.7f;
    }
}

void graphPositionSimulation() {
    globalLogger->info("Physics thread starting");

    const auto simulationInterval = std::chrono::milliseconds(0);

    auto simStart = std::chrono::system_clock::now();
    auto frameStart = simStart;

    while (!shouldTerminate) {
        GS::Graph *readGraph = graphBuf.GetCurrent();
        GS::Graph *writeGraph = graphBuf.GetWriteBuffer();

        auto frameEnd = std::chrono::system_clock::now();
        float elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
        frameStart = frameEnd;

        SimulationControlData dat = controlData.sim.load(std::memory_order_relaxed);

        if (controlData.graph.searching.load(std::memory_order_relaxed)) {
            globalLogger->info("Loading data for " + controlData.graph.searchString);
            writeGraph->Clear();
            search(*writeGraph, controlData.graph.searchString);
            setupGraph(*writeGraph, false);

            graphBuf.PublishAll();
            globalLogger->info("Published graph data");

            readGraph = graphBuf.GetCurrent();
            writeGraph = graphBuf.GetWriteBuffer();

            controlData.graph.searching.store(false, std::memory_order_relaxed);
            controlData.engine.initGraphData.store(true, std::memory_order_relaxed);
            simStart = std::chrono::system_clock::now();
        }

        if (dat.resetSimulation) {
            setupGraph(*writeGraph, false);
            graphBuf.Publish();
            readGraph = graphBuf.GetCurrent();
            writeGraph = graphBuf.GetWriteBuffer();
            dat.resetSimulation = false;
            controlData.sim.store(dat, std::memory_order_relaxed);
            globalLogger->info("Reset simulation");
            simStart = std::chrono::system_clock::now();
        }

        updateGraphPositions(*readGraph, *writeGraph, elapsed_seconds, dat);
        graphBuf.Publish();

        int32_t sourceNode = controlData.graph.sourceNode.load(std::memory_order_relaxed);
        if (sourceNode >= 0) {
            controlData.graph.sourceNode.store(-1, std::memory_order_relaxed);
            std::cout << "Double Clicked on " << sourceNode << std::endl;

            Neo4jInterface neo4jDB("http://127.0.0.1:7474");
            if (!neo4jDB.Authenticate("neo4j", "test1234")) {
                std::cout << "Failed" << std::endl;
            }

            auto linkedPages = neo4jDB.GetLinkedPages(readGraph->nodes.at(sourceNode).title);

            int i = 0;
            for (const auto &page : linkedPages) {
                i++;
                const uint32_t idx = writeGraph->AddNode(page.title.c_str());
                writeGraph->AddEdge(sourceNode, idx);
                writeGraph->nodes[sourceNode].size++;

                if (i > 20) {
                    break;
                }
            }

            graphBuf.Publish();
            readGraph = graphBuf.GetCurrent();
            writeGraph = graphBuf.GetWriteBuffer();
        }

        std::this_thread::sleep_for(simulationInterval);
    }
}

int main() {
    globalLogger->info("WikiMapper starting");

    GS::Graph *writeGraph = graphBuf.GetWriteBuffer();
    setupGraph(*writeGraph);
    graphBuf.PublishAll();

    Engine renderEngine(graphBuf, controlData);

    std::thread t{graphPositionSimulation};

    renderEngine.Run();
    shouldTerminate = true;

    t.join();

    globalLogger->info("WikiMapper exited.");
}
