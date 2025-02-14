#pragma once

#include "shader.hpp" // Your Shader helper class
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

// A simple Label structure.
struct Label {
    glm::vec3 position;
    std::string text; // up to 50 characters
};

// This structure now also stores a copy of the glyph bitmap.
struct LabelCharacter {
    glm::ivec2 size;                         // dimensions of the glyph in pixels
    glm::ivec2 bearing;                      // offset from baseline to left/top of glyph
    unsigned int advance;                    // advance.x is in 1/64 pixels
    std::vector<unsigned char> bitmapBuffer; // copied glyph bitmap (size = width*height)
};

// Per-label billboard vertex data: position, computed half-width (in world units), and texture layer index.
struct LabelData {
    glm::vec3 position;
    float width;    // half-width scaling factor (computed so that full width = 2*vWidth)
    float texIndex; // which layer in the texture array holds this label’s text
};

class LabelEngine {
  public:
    // Constructor loads the font, builds glyph metrics, creates a texture atlas for all labels,
    // and sets up the vertex data so that all labels can be rendered in one draw call.
    LabelEngine(const std::string &fontPath, const std::string &vertexShader, const std::string &fragmentShader,
                const std::string &geometryShader, const std::vector<Label> &labels);

    ~LabelEngine();

    // Call once per frame.
    void RenderLabels(const glm::mat4 &view, const float time);

  private:
    // Builds a texture array atlas from the label texts.
    void CreateTextAtlas(const std::vector<Label> &labels);
    // Creates and uploads the per-label billboard data.
    void PrepareLabels(const std::vector<Label> &labels);

    std::unique_ptr<Shader> m_shader;
    GLuint m_VAO[2]; // [0]: label billboard data, [1]: quad (used in geometry shader)
    GLuint m_VBO[2];
    GLuint m_textAtlas; // GL_TEXTURE_2D_ARRAY holding all label texts

    // Atlas dimensions (all layers have the same width and height)
    int m_atlasWidth, m_atlasHeight;
    int m_atlasLayerCount; // number of labels (layers)

    // Common text metrics computed from the loaded glyphs:
    int m_commonBaseline; // maximum bitmap_top among loaded glyphs
    int m_commonHeight;   // maximum glyph bitmap height

    // Loaded glyphs for the first 128 ASCII characters.
    std::vector<LabelCharacter> m_characters;
    // Per-label billboard vertex data.
    std::vector<LabelData> m_activeLabels;
};
