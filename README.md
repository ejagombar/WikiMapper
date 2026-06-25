# WikiMapper

A real-time 3D graph visualization tool for exploring Wikipedia page connections. WikiMapper renders Wikipedia articles as nodes and their links as edges in an interactive 3D space, powered by force-directed graph algorithms and OpenGL.

[![C++](https://img.shields.io/badge/C%2B%2B-20%2B-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Windows](https://custom-icon-badges.demolab.com/badge/Windows-0078D6?logo=windows11&logoColor=white)](#)
[![Linux](https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=black)](#)

 <img width="1937" height="1134" alt="WikiMapper Screenshot" src="https://github.com/user-attachments/assets/f7c7f4b2-435a-440b-917a-571d2dc7a67d" />

[Demo Video](https://www.youtube.com/watch?v=PC5q615khHg)

<img width="2687" height="1558" alt="Screenshot From 2025-11-05 22-37-14" src="https://github.com/user-attachments/assets/cdce727c-2330-49ef-87c1-9f4846a4f43d" />

## Installation

### Prerequisites
- C++20 compatible compiler
- CMake 3.20 or higher
- OpenGL 4.5+ compatible graphics driver
- **Optional**: Wikipedia [Neo4j database](https://github.com/ejagombar/WikiLoader) *(Public server available at http://eagombar.uk:6348)*

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
# Install dependencies (Example: Ubuntu)
sudo apt-get install libx11-dev xorg-dev libglu1-mesa-dev

# Clone the repository
git clone https://github.com/ejagombar/WikiMapper.git && cd WikiMapper

# Create build directory
mkdir build && cd build

# Configure environment
cmake ..

# Build project
cmake --build .

# Run the application
./WikiMapperExplorer
```

### Build Instructions (Windows)

```bash
# Clone the repository
git clone https://github.com/ejagombar/WikiMapper.git && cd WikiMapper

# Create build directory
mkdir build

# Enter build directory
cd build

# Configure project
cmake .. -G "Visual Studio 17 2022" -A x64

# Build project
cmake --build . --config Release

# Run the application
.\Release\WikiMapperExplorer.exe
```

## Usage

### Basic Controls
- **Right-click + drag** - Rotate camera view
- **WASD** - Move camera position
- **Space / Ctrl** - Move camera up/down
- **Left Shift** - Slower movement
- **Q** - Toggle pause
- **X** - Exit application

### Graph Interaction
- **Left-click** - Select nodes
- **Double-click** - Expand node connections
- **Hover** - Highlight nodes with visual feedback
- **Search bar** - Find and jump to specific Wikipedia pages

### Top Bar
- **Physics** - Dropdown panel for adjusting force-directed simulation parameters: repulsion, attraction, centering force, time step, force multiplier, target distance, and temperature cooling.
- **Rendering** - Dropdown panel for lighting (specular, shininess, ambient), camera (movement speed, mouse sensitivity, field of view), node scale, search result limit, label scale and distance, and display options (V-Sync, FPS counter, size nodes by link count).
- **Database icon** - Pressable button that opens a dropdown for configuring the data source. Supports both an HTTP server and a direct Neo4j database connection. The icon is tinted green when connected and red when disconnected.
- **Node and edge counts** - Live display of the current graph size.
- **FPS counters** - Render FPS and simulation FPS displayed as a stacked pair. Can be hidden from the Rendering panel, which reclaims the space in the bar.

### Pause Mode

Pressing **Q** pauses both the physics simulation and the renderer, significantly reducing CPU and GPU usage.

## Roadmap

### Near-term Features
- [x] Node dragging with physics integration
- [x] Force-directed graph performance optimizations for larger datasets
- [x] Double clicking node causes more relevant nodes to be added to the scene
- [ ] Improve camera movement smoothness

### Long-term Goals
- [ ] Graph clustering and community detection
- [ ] Advanced graph search (shortest path between points)
- [ ] Secondary camera mode (click + drag to rotate visualisation)

## Architecture

### Multi-threading
- **Rendering thread** handles all OpenGL operations and the user interface
- **Physics thread** runs force-directed layout simulation independently, and respects a pause flag to sleep with minimal CPU usage when the application is paused
- **Triple buffer system** ensures thread-safe data exchange without blocking

### Rendering Pipeline
- **Uniform Buffer Objects (UBOs)** for efficient shader data management
- **Instanced rendering** for optimal GPU utilization
- **Custom imposter shaders** for spheres and cylinders to massively reduce vertex count
- **Bloom post-processing** extracts bright pixels at half resolution, applies a multi-pass Gaussian blur, and composites the result back into the scene
- **Framebuffer blur** used for the pause overlay; samples are clamped to edge to prevent wrap-around artifacts at screen borders

### Core Components
- **Graph System** - Node/edge data structures with spatial indexing
- **Physics Simulation** - Barnes-Hut force-directed layout with temperature-based cooling
- **Visual Engine** - OpenGL rendering with modern techniques
- **UI System** - ImGui integration with custom top-bar layout and dropdown panels
- **Database Layer** - Neo4j HTTP API wrapper with live connection status

### Opportunities for Optimisation
There are many potential optimisations with this project. At the moment, the focus is on adding new features and performance is slightly sidelined. Some important performance choices have already been made, such as using imposter spheres instead of sphere meshes, which allows 500,000 spheres to render at over 200 fps.

- [ ] Deferred shading
- [ ] Use cylinder imposters for nearby links and flat lines for distant ones
- [ ] Store node data in contiguous structure-of-arrays layout for fewer cache misses and better SIMD execution
- [x] Prerender text labels to a texture atlas
- [x] Uniform buffer objects for common per-frame shader constants (camera matrices, lighting)

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
