#ifndef LABEL_H
#define LABEL_H

#include "shader.hpp"
#include <cstdint>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../graph.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

struct Label {
    glm::vec3 position;
    std::string text; // up to 50 characters
};

struct LabelCharacter {
    glm::ivec2 size;                   // dimensions of the glyph in pixels
    glm::ivec2 bearing;                // offset from baseline to left/top of glyph
    uint32_t advance;                  // advance.x is in 1/64 pixels
    std::vector<uint8_t> bitmapBuffer; // copied glyph bitmap (size = width*height)
};

struct LabelData {
    glm::vec3 position;
    float width;
    float texIndex;
};

typedef std::vector<uint8_t> Pixels;

struct LabelAtlasData {
    int32_t atlasWidth;
    int32_t atlasHeight;
    std::vector<Pixels> atlases;
};

class LabelEngine {
  public:
    LabelEngine(const std::string &fontPath, const std::string &vertexShader, const std::string &fragmentShader,
                const std::string &geometryShader);

    ~LabelEngine();

    void RenderLabels(const float time);

    std::vector<LabelData> m_activeLabels;

    void SetupTextureAtlases(const std::vector<GS::Node> &nodes);
    void UpdateLabelPositions(const std::vector<GS::Node> &nodes);

    LabelAtlasData PrepareLabelAtlases(const std::vector<GS::Node> &nodes);
    void UploadLabelAtlasesToGPU(const LabelAtlasData &data);

  private:
    std::unique_ptr<Shader> m_shader;
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_textAtlas;

    int32_t m_atlasWidth, m_atlasHeight;
    int32_t m_atlasLayerCount;

    int32_t m_commonBaseline;
    int32_t m_commonHeight;

    std::vector<LabelCharacter> m_characters;
};

#endif // LABEL_H
