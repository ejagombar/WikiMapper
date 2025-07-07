#ifndef LOGGER_H
#define LOGGER_H

#include <memory>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

extern std::shared_ptr<spdlog::logger> globalLogger;

#endif
