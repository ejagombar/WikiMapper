#ifndef TEXTURE_H
#define TEXTURE_H

#include <array>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

#include "../../lib/std_image.h"

GLuint LoadCubemap(const std::array<std::string, 6> &faces);
GLuint LoadCubemap(const std::string &singleFacePath);

GLuint LoadTexture(char const *path);

#endif
