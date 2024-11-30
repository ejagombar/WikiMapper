Still very much under development.

![Wikimapper](https://github.com/user-attachments/assets/f29595ae-86ac-47c5-ad71-a2d21fecbca3)

## Todo

### Features
- [ ] **Oscillating spheres.** Spheres slowly 'wobble' and drift in space to make the graph feel more reactive. Will require modifiying the geometry shader of the spheres and applying an offset over time.
- [ ] **3D graph lines.** Currently a thick line is drawn between nodes on the graph. This line does not change with thickness as the camera gets futher away, making it not look very realistic. An imposter cyclinder needs to be implemented to give each line varying depth and thickness
- [ ] **Menu.** A menu that give the user options to enable/disable features, change the font size etc.
- [ ] **Search.** Allow a user to search for a page and the camera will jump to that node.
- [ ] **External links.** Link each node to the original Wikipedia page.
- [ ] **Lighting**. Add lighting to the spheres shading, and possibly the text and lines as well.
 

### Opportunities for Optimisation
There are many potential optimisations with this project. At the moment, I am focusing on adding new features and performance will be slightly sidelined. Some important performance choices have been made, such as using imposter spheres instead of sphere meshes. This allows for 500,000 of spheres to be created and run at over 200fps. However, many optimisations have been left to later. One of the main reasons for this is to allow me to implement new features faster and eventaully compare the performance between the future optimised version and non-optimised version.

- [ ] Bake the sphere imposter depth maps into a texture, instead of calculating per sphere every frame.
- [ ] Pack the data more tightly that is being sent to the GPU for the imposter spheres. Float values are not required for the positions of the sphere. This can be significantly reduced by having discreet positions in space that the spheres can be placed at which would allow variables smaller than floats to be used. Floats do also not need to be used for the sphere size of colour either.
- [ ] When rendering text, create a texture that contains the word or sentance that needs to be displayed, instead of having a separate draw call for every letter of every word.


This [paper](https://www.cmu.edu/biolphys/deserno/pdf/sphere_equi.pdf) was used for the formula to evenly distribut points around a sphere
https://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere

