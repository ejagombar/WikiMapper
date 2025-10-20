cmake_minimum_required(VERSION 3.20)


include(FetchContent)

project(WikiMapperExplorer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FetchContent_Declare(
  cpp_httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG        v0.20.0
)

FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://github.com/freetype/freetype.git
  GIT_TAG        VER-2-13-2
)

FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v0.1.34
)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3
)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        1.0.1
)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.91.9b-docking
)

FetchContent_Declare(
  spdlog 
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.15.3
)

FetchContent_MakeAvailable(imgui glm glfw glad cpp_httplib freetype spdlog)

find_package(OpenGL REQUIRED)

add_library(imgui_lib STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_lib PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
  ${glfw_SOURCE_DIR}/include
)

file(GLOB_RECURSE Src_Sources "./src/**.cpp" "./src/*.hpp" "./lib/*.cpp" "./lib/*.hpp")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
    file(GLOB SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*")
    file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/")
endif()

file(GLOB_RECURSE SKYBOX_IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
file(COPY ${SKYBOX_IMAGES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${CMAKE_PROJECT_NAME} ${Src_Sources})

# --- Set up include directories for header-only libraries ---
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
  ${glad_SOURCE_DIR}/include
  ${cpp_httplib_SOURCE_DIR}
  ${glm_SOURCE_DIR} 
  ${jsoncpp_SOURCE_DIR}/include
)

# --- Link the downloaded libraries ---
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
  glad
  glfw
  imgui_lib
  nlohmann_json::nlohmann_json
  freetype
  OpenGL::GL
  spdlog::spdlog
)

if(UNIX AND NOT APPLE)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE atomic -static-libasan -static-libstdc++)
endif()

if(MSVC)
  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /W4 /Zi)
else()
  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -g -g3 -O3 -Wall -Wextra -pedantic)
endif()

