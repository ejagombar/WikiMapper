cmake_minimum_required(VERSION 3.27)

project(WikiMapperExplorer)

# Allows clangd to find the library locations
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost 1.85.0 REQUIRED COMPONENTS system asio)
find_package(httplib REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

file(GLOB Src_Sources "./src/*.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${Src_Sources})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
    
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Boost::asio Boost::system)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE httplib::httplib)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE GLEW::GLEW)
# target_link_libraries(${CMAKE_PROJECT_NAME} ${ALL_LIBS})
