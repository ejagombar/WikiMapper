#include "label.hpp"
#include "shader.hpp"
#include <cstdint>
#include <cstring>

LabelEngine::LabelEngine(const std::string &fontPath, const std::string &vertexShader,
                         const std::string &fragmentShader, const std::string &geometryShader) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
        throw std::runtime_error("ERROR::FREETYPE: Failed to load font");

    FT_Set_Pixel_Sizes(face, 0, 128);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    m_characters.resize(128);
    for (uint8_t c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            throw std::runtime_error("ERROR::FREETYTPE: Failed to load Glyph");

        LabelCharacter character;
        character.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        character.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        character.advance = static_cast<uint32_t>(face->glyph->advance.x);

        uint32_t bmpSize = face->glyph->bitmap.width * face->glyph->bitmap.rows;
        character.bitmapBuffer.resize(bmpSize);
        if (bmpSize > 0)
            std::memcpy(character.bitmapBuffer.data(), face->glyph->bitmap.buffer, bmpSize);
        m_characters[c] = character;
    }

    m_commonBaseline = 0;
    m_commonHeight = 0;
    for (auto &ch : m_characters) {
        m_commonBaseline = std::max(m_commonBaseline, static_cast<int32_t>(ch.bearing.y));
        m_commonHeight = std::max(m_commonHeight, static_cast<int32_t>(ch.size.y));
    }
    if (m_commonHeight == 0)
        m_commonHeight = 128; // fallback

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    m_shader = std::make_unique<Shader>(vertexShader, fragmentShader, geometryShader);
    m_shader->LinkUBO("GlobalUniforms", 0);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LabelData), (void *)offsetof(LabelData, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(LabelData), (void *)offsetof(LabelData, width));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(LabelData), (void *)offsetof(LabelData, texIndex));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(LabelData), (void *)offsetof(LabelData, offsetDistance));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(LabelData), (void *)offsetof(LabelData, scale));
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // CreateTextAtlas();
    // UpdateLabelPositions();
}

LabelEngine::~LabelEngine() {
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteTextures(1, &m_textAtlas);
}

void LabelEngine::RenderLabels(const float time) {
    m_shader->Use();
    m_shader->SetFloat("time", time);
    m_shader->SetFloat("vHeight", 0.2f); // The uniform vertical half-size

    // Bind the text atlas texture array (assumed on texture unit 0)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textAtlas);
    m_shader->SetInt("textAtlas", 0);
    m_shader->SetVec3("textColor", glm::vec3(1.0f, 1.0f, 1.0f));

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_activeLabels.size()));
    glBindVertexArray(0);
}

LabelAtlasData LabelEngine::PrepareLabelAtlases(const std::vector<std::string> &nodeTitles) {
    uint32_t numLabels = static_cast<uint32_t>(nodeTitles.size());
    std::vector<int32_t> labelWidths(numLabels, 0);
    int32_t maxWidth = 0;

    for (uint32_t i = 0; i < numLabels; i++) {
        int32_t width = 0;
        const std::string &text = nodeTitles[i];
        for (uint8_t c : text) {
            if (c >= 128)
                continue;
            width += m_characters[c].advance / 64;
        }
        labelWidths[i] = width;
        maxWidth = std::max(maxWidth, width);
    }
    if (maxWidth <= 0)
        maxWidth = 1;

    int32_t atlasWidth = maxWidth;
    int32_t atlasHeight = m_commonHeight;
    size_t layerStride = static_cast<size_t>(atlasWidth) * atlasHeight;

    // Single flat allocation: all layers contiguous so UploadLabelAtlasesToGPU
    // can pass it to glTexImage3D in one call instead of N glTexSubImage3D calls.
    Pixels packed(layerStride * numLabels, 0);

    for (uint32_t i = 0; i < numLabels; i++) {
        uint8_t *layer = packed.data() + i * layerStride;
        int32_t penX = (atlasWidth - labelWidths[i]) / 2;
        int32_t baseline = m_commonBaseline;
        const std::string &text = nodeTitles[i];
        for (uint8_t c : text) {
            if (c >= 128)
                continue;
            const LabelCharacter &ch = m_characters[c];
            int32_t xpos = penX + ch.bearing.x;
            int32_t ypos = baseline - ch.bearing.y;
            for (uint32_t row = 0; row < static_cast<uint32_t>(ch.size.y); row++) {
                for (uint32_t col = 0; col < static_cast<uint32_t>(ch.size.x); col++) {
                    int32_t x = xpos + col;
                    int32_t y = ypos + row;
                    if (x < 0 || x >= atlasWidth || y < 0 || y >= atlasHeight)
                        continue;
                    layer[y * atlasWidth + x] = ch.bitmapBuffer[row * ch.size.x + col];
                }
            }
            penX += ch.advance / 64;
        }
    }

    return {atlasWidth, atlasHeight, numLabels, std::move(packed)};
}

void LabelEngine::UploadLabelAtlasesToGPU(const LabelAtlasData &data) {
    m_atlasWidth = data.atlasWidth;
    m_atlasHeight = data.atlasHeight;
    m_atlasLayerCount = static_cast<int32_t>(data.layerCount);

    glDeleteTextures(1, &m_textAtlas);
    glGenTextures(1, &m_textAtlas);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textAtlas);
    // Upload all layers in one call — previously this was N glTexSubImage3D calls
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, m_atlasWidth, m_atlasHeight, m_atlasLayerCount, 0, GL_RED,
                 GL_UNSIGNED_BYTE, data.packedAtlas.data());
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void LabelEngine::UpdateLabelPositions(const std::vector<uint32_t> &activeNodeIndices,
                                       const std::vector<glm::vec3> &allPositions,
                                       const std::vector<unsigned char> &allSizes,
                                       float sizeMultiplier) {
    uint32_t numLabels = static_cast<uint32_t>(activeNodeIndices.size());
    m_activeLabels.resize(numLabels);
    const float width = (static_cast<float>(m_atlasWidth) / static_cast<float>(m_commonHeight)) * 0.4f;

    float avgSize = 0.0f;

    for (uint32_t i = 0; i < numLabels; i++) {
        avgSize += static_cast<float>(allSizes[activeNodeIndices[i]]);
    }

    if (numLabels > 0) {
        avgSize /= numLabels;
    }

    if (avgSize < 1.0f) {
        avgSize = 1.0f;
    }

    for (uint32_t i = 0; i < numLabels; i++) {
        uint32_t nodeIdx = activeNodeIndices[i];
        m_activeLabels[i].position = allPositions[nodeIdx];
        m_activeLabels[i].width = width;
        m_activeLabels[i].texIndex = static_cast<float>(i);
        m_activeLabels[i].offsetDistance = allSizes[nodeIdx];
        m_activeLabels[i].scale = (static_cast<float>(allSizes[nodeIdx]) / avgSize) * sizeMultiplier;
    }
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, numLabels * sizeof(LabelData), m_activeLabels.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
