#ifndef APPLICATION_H
#define APPLICATION_H

#include "./controlData.hpp"
#include "./store.hpp"
#include <memory>

class ApplicationTasks {
  public:
    ApplicationTasks(std::atomic<bool> &shouldTerminate, ControlData &controlData,
                     std::shared_ptr<dBInterface> &dBInterface, std::mutex &dBInterfaceMutex)
        : m_shouldTerminate(shouldTerminate), m_controlData(controlData), m_dBInterface(dBInterface),
          m_dBInterfaceMutex(dBInterfaceMutex) {};

    void handle();

  private:
    void handle_database_source();
    void handle_search_autocomplete();

    std::atomic<bool> &m_shouldTerminate;
    ControlData &m_controlData;
    std::shared_ptr<dBInterface> m_dBInterface;
    std::mutex &m_dBInterfaceMutex;

    dbInterfaceType m_oldDataSource{};
    std::string m_oldSearchString{};

    static constexpr auto m_sleepInterval = std::chrono::milliseconds(20);
};

#endif
