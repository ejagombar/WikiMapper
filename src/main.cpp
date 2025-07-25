#include "../lib/rgb_hsv.hpp"
#include "controlData.hpp"
#include "graph.hpp"
#include "logger.hpp"
#include "pointMaths.hpp"
#include "simulation.hpp"
#include "spdlog/common.h"
#include "store.hpp"
#include "visual/engine.hpp"
#include <atomic>
#include <cctype>
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
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/std_image.h"

std::shared_ptr<spdlog::logger> globalLogger;

GS::GraphTripleBuf graphBuf;
ControlData controlData;
std::atomic<bool> shouldTerminate(false);

std::shared_ptr<HttpInterface> dBInterface;
std::mutex dBInterfaceMutex;

void generateRealData(GS::Graph &graph) {

    graph.LoadBinary("data2.wiki"); // Use local data for demo
    return;

    std::vector<LinkedPage> linkedPages;

    std::lock_guard<std::mutex> lock(dBInterfaceMutex);

    linkedPages = dBInterface->GetLinkedPages("physics");

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

    linkedPages = dBInterface->GetLinkedPages("multiverse");

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

    linkedPages = dBInterface->GetLinkedPages("atom");

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

    // graph.SaveBinary("data2.wiki");
    // graph.Clear();
    // graph.LoadBinary("data2.wiki"); // Use local data for demo
}

void search(GS::Graph &graph, std::string query) {
    std::vector<LinkedPage> linkedPages;

    std::lock_guard<std::mutex> lock(dBInterfaceMutex);
    linkedPages = dBInterface->GetLinkedPages(query);

    auto x = graph.AddNode(query.c_str());

    for (const auto &page : linkedPages) {
        const uint32_t idx = graph.AddNode(page.title.c_str());
        graph.AddEdge(0, idx);
        graph.nodes.sizes[0]++;
    }

    globalLogger->info("Search query: ", query, " Number of connected nodes: ", graph.nodes.titles.size());
}

void setupGraph(GS::Graph &db, bool genData = true) {
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

std::string toLower(const std::string &input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
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
        double elapsed_seconds = std::chrono::duration<double>(frameEnd - frameStart).count();
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

        int32_t sourceNode = controlData.graph.sourceNode.load(std::memory_order_relaxed);
        if (sourceNode >= 0) {
            controlData.graph.sourceNode.store(-1, std::memory_order_relaxed);

            std::vector<LinkedPage> linkedPages;

            {
                std::lock_guard<std::mutex> lock(dBInterfaceMutex);
                linkedPages = dBInterface->GetLinkedPages(toLower(readGraph->nodes.titles.at(sourceNode)));
            }

            int i = 0;
            for (const auto &page : linkedPages) {
                i++;
                const uint32_t idx = writeGraph->AddNode(page.title);
                writeGraph->AddEdge(sourceNode, idx);
                writeGraph->nodes.sizes[sourceNode]++;

                if (i > 20) {
                    break;
                }
            }

            setupGraph(*writeGraph, false);
            controlData.engine.initGraphData.store(true, std::memory_order_relaxed);

            graphBuf.Publish();
            readGraph = graphBuf.GetCurrent();
            writeGraph = graphBuf.GetWriteBuffer();

            dat.resetSimulation = true;
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

        std::this_thread::sleep_for(simulationInterval);
    }
}

bool isTerminalAttached() { return isatty(STDOUT_FILENO) != 0; }

void initializeLogger(bool enableConsole, bool autoDetectTerminal) {
    std::vector<spdlog::sink_ptr> sinks;

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("wikimapper.log", true);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [thread %t] %v");
    sinks.push_back(file_sink);

    bool shouldAddConsole = enableConsole;
    if (autoDetectTerminal) {
        shouldAddConsole = shouldAddConsole && isTerminalAttached();
    }

    if (shouldAddConsole) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("[%H:%M:%S %z] [%n] [%^%L%$] [thread %t] %v");
        sinks.push_back(console_sink);
    }

    globalLogger = std::make_shared<spdlog::logger>("global", sinks.begin(), sinks.end());

    if (shouldAddConsole) {
        globalLogger->flush_on(spdlog::level::info);
    } else {
        globalLogger->flush_on(spdlog::level::err);
    }

    spdlog::flush_every(std::chrono::seconds(3));
    spdlog::register_logger(globalLogger);
}

int main() {
    initializeLogger(true, true);

    globalLogger->info("WikiMapper starting");

    {
        std::lock_guard<std::mutex> lock(dBInterfaceMutex);
        dBInterface = std::make_shared<HttpInterface>("http://eagombar.uk:6348");

        // dBInterface = std::make_shared<Neo4jInterface>("http://127.0.0.1:7474");
        // if (!dBInterface->Authenticate("neo4j", "test1234")) {
        //     return 1;
        // }
    }

    GS::Graph *writeGraph = graphBuf.GetWriteBuffer();
    setupGraph(*writeGraph);
    graphBuf.PublishAll();

    Engine renderEngine(graphBuf, controlData);

    std::thread t{graphPositionSimulation};

    renderEngine.Run();
    shouldTerminate = true;

    t.join();

    {
        std::lock_guard<std::mutex> lock(dBInterfaceMutex);
        dBInterface.reset();
    }

    globalLogger->info("WikiMapper exited.");
}
