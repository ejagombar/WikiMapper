# WikiMapper
3D graph software to visualise Wikipedia page links. This project is work in progress.

![wikimapper](https://github.com/user-attachments/assets/a2724d92-b34e-4ad5-8d16-27c0a4f778a7)


## Todo

### Quick TODO List
- [x] Set label distance for each node individually (Maybe combine this by storing the number of nodes that each node links to and calculating size, distance from that).
- [ ] Double clicking node causes more relevant nodes to be added to the scene.
- [ ] Click and drag nodes, make it work with simulation.
- [x] Change UI to display only the page title. Find a way to generate a link, somehow without storing the link per node?
- [ ] Fix camera movement smoothness issues. **Unsolved.** 

### Features
- [x] **3D graph lines.** Currently a thick line is drawn between nodes on the graph. This line does not change with thickness as the camera gets futher away, making it not look very realistic. An imposter cyclinder needs to be implemented to give each line varying depth and thickness
- [x] **Menu.** A menu that give the user options to enable/disable features, change the font size etc.
- [x] **Search.** Allow a user to search for a page and the camera will jump to that node.
- [ ] **External links.** Link each node to the original Wikipedia page.
- [x] **Lighting**. Add lighting to the spheres shading, and possibly the text and lines as well.
- [x] **Force Directed Graph Drawing.** Algorithm to spread points out in space. 
 

### Opportunities for Optimisation
There are many potential optimisations with this project. At the moment, I am focusing on adding new features and performance will be slightly sidelined. Some important performance choices have been made, such as using imposter spheres instead of sphere meshes. This allows for 500,000 of spheres to be created and run at over 200fps. However, many optimisations have been left to later. One of the main reasons for this is to allow me to implement new features faster and eventaully compare the performance between the future optimised version and non-optimised version.

- [x] ~~Bake the sphere imposter depth maps into a texture, instead of calculating per sphere every frame.~~ This is barely more efficient than the current method it seems. (Source)[http://11235813tdd.blogspot.com/2013/04/raycasted-spheres-and-point-sprites-vs.html]
- [x] Pack the data more tightly that is being sent to the GPU for the imposter spheres. Float values are not required for the positions of the sphere. This can be significantly reduced by having discreet positions in space that the spheres can be placed at which would allow variables smaller than floats to be used. Floats do also not need to be used for the sphere size of colour either. ** The size and colour has been compressed into a single float for the spheres.**
- [x] When rendering text, create a texture that contains the word or sentance that needs to be displayed, instead of having a separate draw call for every letter of every word.
- [ ] Deferred shading.
- [x] Uniform buffer objects. Some varaibles are constant across shaders every frame, such as the camera position or view and projection matrices. This is a small improvement, and is more about making it clear than increasing performance but using UBOs will mean that they only have to be set once. Not all variables have been moved to this, just the very common ones.
- [x] Simplify sphere imposter frag shader. Remove 4 * and / 2 calculation from quadratic equation. 
- [ ] Use cylinder imposter for close by links and use flat line for futher ones.
- [ ] Store nodes data in contiguous memory for few cache misses and better SIMD execution. For example, node positions could be stored in an array directly as opposed to storing the position as part of a struct in an array. 


## Benchmarks
In order to evaluate the impact that optimisiations and changes that I make have on performance, I have implemented a benchmark system. For now, the benchmark is enabled with some compile time if statements. This benchmark is just a temporary solution and I want to implement it better in the future. Using the compile time #defines allow me to easily add and remove this code. I have added two modes: one mode to record a benchmark path and one to run the benchmark. The benchmark recording mode records the movement of the camera and appends the position to a file. A second file is used to store timestamps of when the benchmark is stopped and started. This allows me to benchmark different scenarios separately. The second mode allows me to replay the recorded file and disable user input. The number of frames are recorded for each section and divided by the total time that it took to run that section. Two benchmark files have been added to the assets folder to allow me to test the code across multiple machines. 

### Benchmark Setup
- **Section 1** - Sphere (node) closeup.
- **Section 2** - View of the whole graph.
- **Section 3** - Closeup of a collection of cylinders (verticies).
- **Section 4** - Facing away from the graph, with no objects in view.

The below benchmarks were run on my computer. The frame time measurements are recorded in milliseconds.

| Commit Tag | 1 | 2 | 3 | 4 | Overall | Notable Changes |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | 
|69967a7|1.395|1.701|1.382|1.269|1.437|Baseline|
|8122499|1.287|1.679|1.303|1.250|1.380| Simplify imposter cylinder shaders.|

This [paper](https://www.cmu.edu/biolphys/deserno/pdf/sphere_equi.pdf) was used for the formula to evenly distribut points around a sphere
https://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere

