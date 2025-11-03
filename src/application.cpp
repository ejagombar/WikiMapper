
#include "./application.hpp"
#include "./logger.hpp"
#include "controlData.hpp"
#include <mutex>
#include <unistd.h>

void ApplicationTasks::handle() {

    while (!m_shouldTerminate) {
        std::this_thread::sleep_for(m_sleepInterval);

        handle_database_source();
        handle_search_autocomplete();
    }
}

void ApplicationTasks::handle_database_source() {

    std::lock_guard<std::mutex> lock(m_controlData.app.dataSourceMutex);

    if (m_controlData.app.dataSource.attemptDataConnection == true ||
        m_controlData.app.dataSource.sourceType != m_oldDataSource) {

        m_controlData.app.dataSource.connectedToDataSource = false;
        std::lock_guard<std::mutex> lock2(m_dBInterfaceMutex);

        if (m_controlData.app.dataSource.sourceType == dbInterfaceType::DB) {
            globalLogger->info("Database source switching to local DB", m_controlData.app.dataSource.dbUrl);

            m_dBInterface = std::make_shared<Neo4jInterface>(m_controlData.app.dataSource.dbUrl);

            if (!m_dBInterface->Authenticate(m_controlData.app.dataSource.dbUsername,
                                             m_controlData.app.dataSource.dbPassword)) {
                globalLogger->info("Failed to Auth. URL: {}", m_controlData.app.dataSource.dbUrl);
            }

        } else if (m_controlData.app.dataSource.sourceType == dbInterfaceType::HTTP) {
            globalLogger->info("Database source switching to HTTP server", m_controlData.app.dataSource.dbUrl);

            m_dBInterface = std::make_shared<HttpInterface>(m_controlData.app.dataSource.serverUrl);
        }

        m_controlData.app.dataSource.connectedToDataSource = m_dBInterface->connected();

        m_controlData.app.dataSource.attemptDataConnection = false;
        globalLogger->info("Database connected {}", m_controlData.app.dataSource.connectedToDataSource);
    }

    m_oldDataSource = m_controlData.app.dataSource.sourceType;
}

void ApplicationTasks::handle_search_autocomplete() {
    if (m_oldSearchString == m_controlData.graph.searchString) {
        std::lock_guard<std::mutex> lock2(m_dBInterfaceMutex);
    }

    m_oldSearchString = m_controlData.graph.searchString;
}
