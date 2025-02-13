#include "label.hpp"
#include "shader.hpp"
#include <glm/common.hpp>
#include <iostream>
#include <stdexcept>

LabelEngine::LabelEngine(const std::string &fontPath, const std::string &vertexShader,
                         const std::string &fragmentShader, const std::string &geometryShader,
                         const std::vector<Label> &labels) {

    // FT_Library ft;
    // if (FT_Init_FreeType(&ft)) {
    //     throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");
    // }
    //
    // if (FT_New_Face(ft, fontPath.c_str(), 0, &m_face)) {
    //     throw std::runtime_error("ERROR::FREETYPE: Failed to load font");
    // }
    //
    // FT_Set_Pixel_Sizes(m_face, 0, 128);
    //
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    // FT_GlyphSlot slot = m_face->glyph;
    //
    // for (unsigned char c = 0; c < 128; c++) {
    //     // load character glyph
    //     if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) {
    //         throw std::runtime_error("ERROR::FREETYTPE: Failed to load Glyph");
    //     }
    //
    //     FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);
    //
    //     // generate texture
    //     unsigned int texture;
    //     glGenTextures(1, &texture);
    //     glBindTexture(GL_TEXTURE_2D, texture);
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows, 0, GL_RED,
    //                  GL_UNSIGNED_BYTE, m_face->glyph->bitmap.buffer);
    //     // set texture options
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //     // now store character for later use
    //     LabelCharacter character = {texture, glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
    //                                 glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
    //                                 static_cast<uint>(m_face->glyph->advance.x)};
    //
    //     m_characters.insert(std::pair<char, LabelCharacter>(c, character));
    // }
    //
    // FT_Done_Face(m_face);
    // FT_Done_FreeType(ft);

    m_shader = std::make_unique<Shader>(vertexShader, fragmentShader, geometryShader);
    m_shader->LinkUBO("GlobalUniforms", 0);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    //
    // glBindVertexArray(m_VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);
    //
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    // glEnableVertexAttribArray(0);
    //
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    //
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    //
    // glBindVertexArray(0);

    PrepareLabels(labels);
}

void LabelEngine::PrepareLabels(const std::vector<Label> &labels) {

    for (int i = 0; i < m_maxActiveLabels; i++) {
        if (i < labels.size()) {
            m_activeLabels[i].position[0] = labels[i].position.x;
            m_activeLabels[i].position[1] = labels[i].position.y;
            m_activeLabels[i].position[2] = labels[i].position.z;
            std::cout << labels[i].position.z << std::endl;
            m_activeLabels[i].width = 2.0f;
        } else {
            m_activeLabels[i].position[0] = 0.0f;
            m_activeLabels[i].position[1] = 0.0f;
            m_activeLabels[i].position[2] = 0.0f;
            m_activeLabels[i].width = 0.0f;
        }
    }

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_activeLabels), m_activeLabels, GL_STATIC_DRAW);

    const GLint aWidthAttrib = m_shader->GetAttribLocation("aWidth");
    glEnableVertexAttribArray(aWidthAttrib);
    glVertexAttribPointer(aWidthAttrib, 1, GL_FLOAT, GL_FALSE, sizeof(LabelData), (void *)0);

    const GLint aCoordAttrib = m_shader->GetAttribLocation("aCoord");
    glEnableVertexAttribArray(aCoordAttrib);
    glVertexAttribPointer(aCoordAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(LabelData), (void *)(sizeof(float)));

    glBindVertexArray(0);
}

LabelEngine::~LabelEngine() {
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void LabelEngine::RenderLabels(const glm::mat4 &view, const float time) {

    m_shader->Use();
    m_shader->SetFloat("time", time);
    m_shader->SetFloat("vHeight", 0.2f);

    // Bind the texture atlas (assume it’s on texture unit 0)
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, atlasTexture);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_POINTS, 0, m_maxActiveLabels);
    glBindVertexArray(0);
}
