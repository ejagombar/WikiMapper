#ifndef LABEL_H
#define LABEL_H

#include "shader.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

struct Label {
    glm::vec3 position;
    std::string text; // up to 50 characters
};

struct LabelCharacter {
    glm::ivec2 size;                         // dimensions of the glyph in pixels
    glm::ivec2 bearing;                      // offset from baseline to left/top of glyph
    unsigned int advance;                    // advance.x is in 1/64 pixels
    std::vector<unsigned char> bitmapBuffer; // copied glyph bitmap (size = width*height)
};

struct LabelData {
    glm::vec3 position;
    float width;
    float texIndex;
};

class LabelEngine {
  public:
    LabelEngine(const std::string &fontPath, const std::string &vertexShader, const std::string &fragmentShader,
                const std::string &geometryShader, const std::vector<Label> &labels);

    ~LabelEngine();

    void RenderLabels(const glm::mat4 &view, const float time);

  private:
    void CreateTextAtlas(const std::vector<Label> &labels);
    void PrepareLabels(const std::vector<Label> &labels);

    std::unique_ptr<Shader> m_shader;
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_textAtlas;

    int m_atlasWidth, m_atlasHeight;
    int m_atlasLayerCount;

    int m_commonBaseline;
    int m_commonHeight;

    std::vector<LabelCharacter> m_characters;
    std::vector<LabelData> m_activeLabels;
};

#endif // LABEL_H
