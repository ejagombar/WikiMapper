#ifndef TEXT_H
#define TEXT_H

#include <GLFW/glfw3.h>
#include <ft2build.h>
#include <glm/gtc/type_ptr.hpp>
#include <map>

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    long Advance;           // Offset to advance to next glyph NOTE: modified from uint
};

class Text2d {
  public:
    int generateCharacters();
    std::map<char, Character> Characters;
};

#endif
