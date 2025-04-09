In order to visualise the data extracted from the Wikipedia XML file into Neo4j, a graph explorer is required. Although visualisers developed by Neo4j do exist, the free projects are not able to visualise the data in 3D and also it would be more interesting to develop it myself.

I decided to use OpenGL for this project. Other options included Vulkan, however this is much more complicated than OpenGL and only really brings real benefits if you are experienced and know what you are doing. I do not.

## Following OpenGL Tutorials
I followed the [opengl-tutorial.org](http://www.opengl-tutorial.org/beginners-tutorials/tutorial-1-opening-a-window/) tutorials which gave a good introduction to OpenGL. Tutorial 18: Billboards and Particles was particularly helpful and provided the base of the project.

I made a start on the project with the knowledge I had learnt from that tutorial. I got some basic sphere billboards workingk, however the lighting was incorrect and they did not have any proper
depth, making them look unrealistic when the lines were places near them.

I went back to the basics and followed this tutorial series [learnopengl.com](https://learnopengl.com/Introduction).

Following the learnOpenGLtutorial was definitely worth it. I gained a decent amount of understanding from the first tutorials series that I followed but I realised that I my progress on the project was slowing down and I was reaching roadblocks. Following these tutorial helped me learn how OpenGL works on a deeper level. It also gave me some inspiration for new things to add to the project and some improvements.

### Sphere Imposters
Struggled with this for a long time. Here are some resources that I used:

 - [Chapter 13. Lies and Impostors](https://paroj.github.io/gltut/Illumination/Tutorial%2013.html)
 - [Rendering a Sphere on a Quad](https://bgolus.medium.com/rendering-a-sphere-on-a-quad-13c92025570c)
 - [Ambient Occlusion and Edge Cueing to Enhance Real Time Molecular Visualization](http://vcg.isti.cnr.it/Publications/2006/TCM06/Tarini_FinalVersionElec.pdf)
 - [Point Sprite Sphere Impostors \[2023-06-21\]](https://www.youtube.com/watch?v=a8R8ZxPy3eA)
 - [Drawing-Millions-Of-Spheres](https://community.khronos.org/t/drawing-millions-of-sphere/62742)
 - [OpenGL impostor-sphere : problem when calculating the depth value](https://stackoverflow.com/questions/53650693/opengl-impostor-sphere-problem-when-calculating-the-depth-value)
 - [Learning GLSL: the goal](https://github.com/ssloy/glsltuto/tree/master)
 - [Raycasted Spheres and Point Sprites vs. Geometry Instancing (OpenGL 3.3)](http://11235813tdd.blogspot.com/2013/04/raycasted-spheres-and-point-sprites-vs.html)
 - [Imposter Sphere](https://community.khronos.org/t/imposter-sphere/71189)


## Explorer Dev Update

I have not made an update for a long time however have still been developing the OpenGL explorer. I have now implemented Imposter spheres and Cylinders which was quite a challenge.
Furthermore, I have worked on adding lighting to both these shaders, with simple Phong shading tecniques. I have also implemented a very basic text drawing class, however it is currently horribly optimised as I have a separate draw call for every single letter. This is obviously the performance bottleneck at the moment so this will be worked on at some point, however it works for now. A full list of features can be seen in the Explorer readme or by looking at the commit logs.

The next steps for the project is to integrate the Neo4j database into the explorer project so I can pull down some real data with real links. I do have some basic test data but it would be useful to have the actual data as I am soon to be working on the algorithm that will layout the graph in 3d space.

## Graph 
