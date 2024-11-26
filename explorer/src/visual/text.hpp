#include "./shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

#include <unordered_map>

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

class Text {
  public:
    Text();
    ~Text() {};

    void Render(std::string text, glm::vec3 position, glm::mat4 projection, float scale, glm::vec3 color);

  private:
    FT_Face m_face;
    GLuint m_VAO, m_VBO;
    std::unordered_map<char, Character> m_characters;

    std::unique_ptr<Shader> m_textShader;
};
