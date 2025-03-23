#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "../../lib/std_image.h"

GLuint LoadCubemap(std::vector<std::string> faces);

GLuint LoadTexture(char const *path);

#endif
