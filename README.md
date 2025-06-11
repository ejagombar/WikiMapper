# WikiMapper

A real-time 3D graph visualization tool for exploring Wikipedia page connections. WikiMapper renders Wikipedia articles as nodes and their links as edges in an interactive 3D space, powered by force-directed graph algorithms and OpenGL.

[![C++](https://img.shields.io/badge/C%2B%2B-20%2B-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

![Screenshot From 2025-06-12 00-09-59](https://github.com/user-attachments/assets/3e9e6b5e-b7fd-4584-b559-1ed1513fd0ab)

## Installation

### Prerequisites
- C++20 compatible compiler 
- CMake 3.20 or higher
- OpenGL 4.5+ compatible graphics driver
- **Optional**: Wikipedia [Neo4j database](https://github.com/ejagombar/WikiLoader) *(Public server coming soon)*

### Dependencies
All dependencies are automatically fetched by CMake:
- OpenGL/GLAD for graphics
- GLFW for windowing
- GLM for mathematics
- Dear ImGui for UI
- FreeType for text rendering
- cpp-httplib for HTTP requests
- spdlog for logging
- nlohmann/json for JSON parsing

### Build Instructions (Linux)

```bash
# Clone the repository
git clone https://github.com/ejagombar/WikiMapper.git && cd wikimapper

# Create build directory
mkdir build

# Configure environment
cmake --preset=default

# Build project
cmake --build build

# Run the application
./build/WikiMapperExplorer
```

Windows coming soon...

## Usage

### Basic Controls
- **Right-click + drag** - Rotate camera view
- **WASD** - Move camera position
- **Space/Ctrl** - Move camera up/down
- **Left Shift** - Crouch mode for slower movement
- **Q** - Toggle settings menu
- **X** - Exit application

### Graph Interaction
- **Left-click** - Select nodes
- **Double-click** - Expand node connections
- **Hover** - Highlight nodes with visual feedback
- **Search bar** - Find and jump to specific Wikipedia pages

## Roadmap

### Near-term Features
- [ ] Node dragging with physics integration
- [ ] Force-directed graph performance optimizations for larger datasets
- [x] Double clicking node causes more relevant nodes to be added to the scene
- [ ] Improve camera movement smoothness

### Long-term Goals
- [ ] Graph clustering and community detection
- [ ] Advanced graph search (shortest path between points)
- [ ] Secondary camera mode (click + drag to rotate visualisation)

## Architecture

### Multi-threading
- **Rendering thread** handles all OpenGL operations and user interface
- **Physics thread** runs force-directed layout simulation independently
- **Triple buffer system** ensures thread-safe data exchange without blocking

### Rendering Pipeline
- **Uniform Buffer Objects (UBOs)** for efficient shader data management
- **Instanced rendering** for optimal GPU utilization
- **Custom Imposter shaders** for sphere and cylinder to massively reduce vertex count
- **Framebuffer effects** including blur filters and post-processing

### Core Components
- **Graph System** - Node/edge data structures with spatial indexing
- **Physics Simulation** - Force-directed layout with oscillation detection
- **Visual Engine** - OpenGL rendering with modern techniques
- **UI System** - ImGui integration with custom styling
- **Database Layer** - Neo4j HTTP API wrapper

### Opportunities for Optimisation
There are many potential optimisations with this project. At the moment, I am focusing on adding new features and performance will be slightly sidelined. Some important performance choices have been made, such as using imposter spheres instead of sphere meshes. This allows for 500,000 of spheres to be created and run at over 200fps. However, many optimisations have been left to later. One of the main reasons for this is to allow me to implement new features faster and eventaully compare the performance between the future optimised version and non-optimised version.

- [ ] Deferred shading.
- [ ] Use cylinder imposters for close by links and use flat line for futher ones.
- [ ] Store nodes data in contiguous memory for few cache misses and better SIMD execution. For example, node positions could be stored in an array directly as opposed to storing the position as part of a struct in an array. 
- [x] Prerender text labels to a texture.
- [x] Uniform buffer objects. Some varaibles are constant across shaders every frame, such as the camera position or view and projection matrices. This is a small improvement, and is more about making it clear than increasing performance but using UBOs will mean that they only have to be set once. Not all variables have been moved to this, just the very common ones.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Credits and Sources

### Sphere Imposter Techniques
- [Ambient Occlusion and Edge Cueing to Enhance Real Time Molecular Visualization](http://vcg.isti.cnr.it/Publications/2006/TCM06/Tarini_FinalVersionElec.pdf) - Foundational paper on sphere imposters for molecular visualization
- [Rendering a Sphere on a Quad](https://bgolus.medium.com/rendering-a-sphere-on-a-quad-13c92025570c) - Ben Golus's comprehensive guide to sphere imposters
- [Lies and Impostors](https://paroj.github.io/gltut/Illumination/Tutorial%2013.html) - Detailed explanation of imposter geometry techniques
- [Point Sprite Sphere Impostors](https://www.youtube.com/watch?v=a8R8ZxPy3eA) - Video tutorial on modern sphere imposter implementation
- [Raycasted Spheres and Point Sprites vs. Geometry Instancing (OpenGL 3.3)](http://11235813tdd.blogspot.com/2013/04/raycasted-spheres-and-point-sprites-vs.html) - Performance comparison and optimization insights
- [OpenGL impostor-sphere: problem when calculating the depth value](https://stackoverflow.com/questions/53650693/opengl-impostor-sphere-problem-when-calculating-the-depth-value) - Community solutions for depth buffer issues
- [Drawing Millions of Spheres](https://community.khronos.org/t/drawing-millions-of-sphere/62742) - Khronos community discussion on large-scale sphere rendering
- [Imposter Sphere](https://community.khronos.org/t/imposter-sphere/71189) - Additional community insights and implementation details

### General Guides

- [learnopengl.com](https://learnopengl.com/) - Comprehensive modern OpenGL tutorials that provided the foundation for the rendering pipeline
- [opengl-tutorial.org](http://www.opengl-tutorial.org/) - Initial OpenGL tutorials, particularly Tutorial 18: Billboards and Particles
- [Learning GLSL](https://github.com/ssloy/glsltuto/tree/master) - Shader programming techniques and best practices

### Libraries and Dependencies
- [GLFW](https://github.com/glfw/glfw) - Cross-platform window management and input handling
- [GLAD](https://github.com/Dav1dde/glad) - OpenGL function loader
- [GLM](https://github.com/g-truc/glm) - Mathematics library for graphics software
- [Dear ImGui](https://github.com/ocornut/imgui) - Immediate mode GUI library with custom styling
- [FreeType](https://github.com/freetype/freetype) - Font rendering library for high-quality text
- [cpp-httplib](https://github.com/yhirose/cpp-httplib) - HTTP client library
- [nlohmann/json](https://github.com/nlohmann/json) - C++ JSON library
- [spdlog](https://github.com/gabime/spdlog) - C++ logging library

### Data Sources
- [Wikimedia Foundation](https://www.wikimedia.org/) - Wikipedia data and infrastructure
