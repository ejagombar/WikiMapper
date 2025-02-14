#ifndef LABEL_H
#define LABEL_H

#include "./shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include <vector>
#include FT_FREETYPE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct LabelCharacter {
    GLuint TextureID;   // (Not used in batched rendering – we assume an atlas)
    glm::ivec2 Size;    // Size of glyph in pixels
    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
    GLuint Advance;     // Offset to advance to next glyph
};

struct Label {
    glm::vec3 position;
    std::string text;
};

class LabelEngine {
  public:
    LabelEngine(const std::string &fontPath, const std::string &vertexShader, const std::string &fragmentShader,
                const std::string &geometryShader, const std::vector<Label> &labels);
    ~LabelEngine();

    void CreateLetterTexture();
    void SetupLabelShader();
    void PrepareLabels(const std::vector<Label> &labels);

    void AddLabel(const std::string &text, const glm::vec3 &position);
    void RenderLabels(const glm::mat4 &view, const float time);

    std::unique_ptr<Shader> m_shader;

  private:
    struct LabelData {
        GLfloat width;
        GLfloat position[3];
    };

    static const uint m_maxActiveLabels = 1000;

    LabelData m_activeLabels[m_maxActiveLabels];

    FT_Face m_face;
    GLuint m_VAO[2], m_VBO[2];
    std::vector<LabelCharacter> m_characters;
};

#endif
