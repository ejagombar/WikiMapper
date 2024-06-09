cmake_minimum_required(VERSION 3.27)

project(WikiMapper)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(re2 REQUIRED)

# Then use pkg-config for locate specific package
pkg_check_modules(libxmlpp REQUIRED IMPORTED_TARGET libxml++-5.0)


add_executable(${CMAKE_PROJECT_NAME} src/main.cpp src/myparser.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE PkgConfig::libxmlpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE re2::re2)
