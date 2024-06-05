cmake_minimum_required(VERSION 3.27)

project(WikiMapper)

find_package(fmt CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)

# Then use pkg-config for locate specific package
pkg_check_modules(libxmlpp REQUIRED IMPORTED_TARGET libxml++-5.0)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE PkgConfig::libxmlpp)