cmake_minimum_required(VERSION 3.27)

project(WikiMapperExplorer)

# Allows clangd to find the library locations
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost 1.85.0 REQUIRED COMPONENTS system asio)

file(GLOB Src_Sources "./src/*.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${Src_Sources})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Boost::asio Boost::system)
