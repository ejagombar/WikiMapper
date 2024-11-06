#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "./std_image.h"

unsigned int LoadCubemap(std::vector<std::string> faces);

unsigned int LoadTexture(char const *path);
