cmake_minimum_required(VERSION 3.27)
project(WikiMapper)


find_package(LIBXMLPP REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} libxmlpp::libxmlpp)
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
