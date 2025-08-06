#include "controlData.hpp"
#include "graph.hpp"
#include "logger.hpp"
#include "simulation.hpp"
#include "spdlog/common.h"
#include "store.hpp"
#include "visual/engine.hpp"
#include <atomic>
#include <chrono>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <json/json.h>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/std_image.h"

std::shared_ptr<spdlog::logger> globalLogger;

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
    ControlData controlData;
    GS::GraphTripleBuf graphBuf;

    std::mutex dBInterfaceMutex;
    std::shared_ptr<dBInterface> dBInterface;

    std::atomic<bool> shouldTerminate(false);

    initializeLogger(true, true);
    // initializeLogger(false, false);

    globalLogger->info("WikiMapper starting");
    {
        std::lock_guard<std::mutex> lock(dBInterfaceMutex);
        dBInterface = std::make_shared<HttpInterface>("http://eagombar.uk:6348");
        // dBInterface = std::make_shared<Neo4jInterface>("http://127.0.0.1:7474");

        if (dBInterface->RequiresAuthentication() && !dBInterface->Authenticate("neo4j", "test1234")) {
            globalLogger->info("Failed to Auth");
            return 1;
        }
    }

    GraphEngine graphEngine(graphBuf, shouldTerminate, controlData, dBInterface, dBInterfaceMutex);

    GS::Graph *writeGraph = graphBuf.GetWriteBuffer();
    graphEngine.setupGraph(*writeGraph);
    graphBuf.PublishAll();

    Engine renderEngine(graphBuf, controlData);

    std::thread t{&GraphEngine::graphPositionSimulation, std::ref(graphEngine)};

    renderEngine.Run();

    // writeGraph->SaveBinary("physics.wiki");

    shouldTerminate = true;
    t.join();

    {
        std::lock_guard<std::mutex> lock(dBInterfaceMutex);
        dBInterface.reset();
    }

    globalLogger->info("WikiMapper exited.");
}
