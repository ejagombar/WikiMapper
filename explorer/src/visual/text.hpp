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
    Text(const std::string font, const std::string vertexShader, const std::string fragmentShader);
    ~Text() {};

    void SetTransforms(const glm::mat4 projection, const glm::mat4 view, const glm::vec3 cameraPosition);
    void Render(const std::string text, const glm::vec3 position, const float scale, const glm::vec3 color);

  private:
    FT_Face m_face;
    GLuint m_VAO, m_VBO;
    std::unordered_map<char, Character> m_characters;

    std::unique_ptr<Shader> m_textShader;
};

class Text2d : public Text {
  public:
    Text2d(const std::string font, const std::string vertexShader, const std::string fragmentShader)
        : Text(font, vertexShader, fragmentShader) {};

    void Render2d(const std::string text, const glm::vec3 position, const float scale, const glm::vec3 color);
    void UpdateScreenSize(const float width, const float height);

  private:
    glm::mat4 m_projection;
};
