cmake_minimum_required(VERSION 3.27)


project(WikiMapperExplorer)

# Allows clangd to find the library locations
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)
find_package(Freetype REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(httplib CONFIG REQUIRED)

file(GLOB_RECURSE Src_Sources "./src/**.cpp" "./src/*.hpp" "./lib/*.cpp" "./lib/*.hpp")

file(GLOB SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*")
file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Ensure that CMake reruns if any shaders are updated. This means that they are copied across to the build dir
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/")

file(GLOB_RECURSE SKYBOX_IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
file(COPY ${SKYBOX_IMAGES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${CMAKE_PROJECT_NAME} ${Src_Sources})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${FREETYPE_INCLUDE_DIRS})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glad::glad)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE httplib::httplib)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${FREETYPE_LIBRARIES})

# target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)
# target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Boost::asio Boost::system)
