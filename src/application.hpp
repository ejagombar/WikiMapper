#ifndef APPLICATION_H
#define APPLICATION_H

#include "./controlData.hpp"
#include "./store.hpp"

void handle_database_source(ControlData &controlData, std::shared_ptr<dBInterface> &dBInterface,
                            std::mutex &dBInterfaceMutex);

void handle_application_tasks(std::atomic<bool> &shouldTerminate, ControlData &controlData,
                              std::shared_ptr<dBInterface> &dBInterface, std::mutex &dBInterfaceMutex);

#endif
