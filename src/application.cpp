
#include "./application.hpp"
#include "./logger.hpp"
#include "controlData.hpp"
#include <unistd.h>

const auto sleepInterval = std::chrono::milliseconds(100);

void handle_application_tasks(std::atomic<bool> &shouldTerminate, ControlData &controlData,
                              std::shared_ptr<dBInterface> &dBInterface, std::mutex &dBInterfaceMutex) {

    while (!shouldTerminate) {
        std::this_thread::sleep_for(sleepInterval);

        handle_database_source(controlData, dBInterface, dBInterfaceMutex);
    }
}

void handle_database_source(ControlData &controlData, std::shared_ptr<dBInterface> &dBInterface,
                            std::mutex &dBInterfaceMutex) {

    static dbInterfaceType oldDataSource;

    std::lock_guard<std::mutex> lock(controlData.app.dataSourceMutex);

    if (controlData.app.dataSource.attemptDataConnection == true ||
        controlData.app.dataSource.sourceType != oldDataSource) {

        controlData.app.dataSource.connectedToDataSource = false;
        std::lock_guard<std::mutex> lock(dBInterfaceMutex);

        if (controlData.app.dataSource.sourceType == dbInterfaceType::DB) {
            globalLogger->info("Database source switching to local DB", controlData.app.dataSource.dbUrl);

            dBInterface = std::make_shared<Neo4jInterface>(controlData.app.dataSource.dbUrl);

            if (!dBInterface->Authenticate(controlData.app.dataSource.dbUsername,
                                           controlData.app.dataSource.dbPassword)) {
                globalLogger->info("Failed to Auth. URL: {}", controlData.app.dataSource.dbUrl);
            }

        } else if (controlData.app.dataSource.sourceType == dbInterfaceType::HTTP) {
            globalLogger->info("Database source switching to HTTP server", controlData.app.dataSource.dbUrl);

            dBInterface = std::make_shared<HttpInterface>(controlData.app.dataSource.serverUrl);
        }

        controlData.app.dataSource.connectedToDataSource = dBInterface->connected();

        controlData.app.dataSource.attemptDataConnection = false;
        globalLogger->info("Database connected {}", controlData.app.dataSource.connectedToDataSource);
    }

    oldDataSource = controlData.app.dataSource.sourceType;
}
