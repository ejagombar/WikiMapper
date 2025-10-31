#include "application.hpp"
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

constexpr std::string SETTINGS_FILE = "settings.json";

std::shared_ptr<spdlog::logger> globalLogger;

void initializeLogger(bool enableConsole) {
    std::vector<spdlog::sink_ptr> sinks;

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("wikimapper.log", true);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [thread %t] %v");
    sinks.push_back(file_sink);

    if (enableConsole) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("[%H:%M:%S %z] [%n] [%^%L%$] [thread %t] %v");
        sinks.push_back(console_sink);
    }

    globalLogger = std::make_shared<spdlog::logger>("global", sinks.begin(), sinks.end());

    if (enableConsole) {
        globalLogger->flush_on(spdlog::level::info);
    } else {
        globalLogger->flush_on(spdlog::level::err);
    }

    spdlog::flush_every(std::chrono::seconds(3));
    spdlog::register_logger(globalLogger);
}

int main() {
    ControlData controlData;

    std::mutex dBInterfaceMutex;
    std::shared_ptr<dBInterface> dBInterface;

    initializeLogger(true);

    globalLogger->info("WikiMapper starting");

    if (controlData.LoadControlData(SETTINGS_FILE)) {
        globalLogger->info("Loaded settings");
    }

    {
        std::lock_guard<std::mutex> lock(dBInterfaceMutex);

        if (controlData.app.dataSource.sourceType == dbInterfaceType::DB) {
            dBInterface = std::make_shared<Neo4jInterface>(controlData.app.dataSource.dbUrl);

            if (!dBInterface->Authenticate(controlData.app.dataSource.dbUsername,
                                           controlData.app.dataSource.dbPassword)) {
                globalLogger->info("Failed to Auth. URL: {}", controlData.app.dataSource.dbUrl);
                // return 1;
            }
        } else if (controlData.app.dataSource.sourceType == dbInterfaceType::HTTP) {
            dBInterface = std::make_shared<HttpInterface>(controlData.app.dataSource.serverUrl);
        }

        controlData.app.dataSource.connectedToDataSource = dBInterface->connected();
        globalLogger->info("Connected : {}", controlData.app.dataSource.connectedToDataSource);
    }

    GS::GraphTripleBuf graphBuf;
    std::atomic<bool> shouldTerminate(false);

    GraphEngine graphEngine(graphBuf, shouldTerminate, controlData, dBInterface, dBInterfaceMutex);

    GS::Graph *writeGraph = graphBuf.GetWriteBuffer();
    graphEngine.setupGraph(*writeGraph);
    graphBuf.PublishAll();

    RenderEngine renderEngine(graphBuf, controlData);

    std::thread t{&GraphEngine::graphPositionSimulation, std::ref(graphEngine)};

    std::thread d{handle_application_tasks, std::ref(shouldTerminate), std::ref(controlData), std::ref(dBInterface),
                  std::ref(dBInterfaceMutex)};

    renderEngine.Run();

    // writeGraph->SaveBinary("physics.wiki");

    shouldTerminate = true;
    t.join();
    d.join();

    {
        std::lock_guard<std::mutex> lock(dBInterfaceMutex);
        dBInterface.reset();
    }

    controlData.SaveControlData(SETTINGS_FILE);

    globalLogger->info("WikiMapper exited.");
}
