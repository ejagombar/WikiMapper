cmake_minimum_required(VERSION 3.27)

project(WikiMapperExplorer)

# Allows clangd to find the library locations
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost 1.85.0 REQUIRED COMPONENTS system asio)
find_package(httplib REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

file(GLOB Src_Sources "./src/*.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${Src_Sources})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
    
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Boost::asio Boost::system)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE httplib::httplib)
