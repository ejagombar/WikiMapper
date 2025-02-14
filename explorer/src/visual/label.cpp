// render_engine.cpp
#include "label.hpp"
#include <cstring> // for memcpy

LabelEngine::LabelEngine(const std::string &fontPath, const std::string &vertexShader,
                         const std::string &fragmentShader, const std::string &geometryShader,
                         const std::vector<Label> &labels) {
    // 1. Initialize FreeType and load the font face.
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
        throw std::runtime_error("ERROR::FREETYPE: Failed to load font");

    // Set font pixel size. (Here we choose 128 so that glyph bitmaps are large enough.)
    FT_Set_Pixel_Sizes(face, 0, 128);

    // Disable byte-alignment restriction.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 2. Load first 128 ASCII glyphs.
    m_characters.resize(128);
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            throw std::runtime_error("ERROR::FREETYTPE: Failed to load Glyph");

        // Generate texture for individual glyph (if you need it later).
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,
                     GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        LabelCharacter character;
        character.TextureID = texture;
        character.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        character.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        character.advance = static_cast<unsigned int>(face->glyph->advance.x);
        // Save a copy of the glyph bitmap.
        int bmpSize = face->glyph->bitmap.width * face->glyph->bitmap.rows;
        character.bitmapBuffer.resize(bmpSize);
        if (bmpSize > 0)
            std::memcpy(character.bitmapBuffer.data(), face->glyph->bitmap.buffer, bmpSize);
        m_characters[c] = character;
    }

    // Compute a common baseline and common height (so that every label texture has the same height).
    m_commonBaseline = 0;
    m_commonHeight = 0;
    for (auto &ch : m_characters) {
        m_commonBaseline = std::max(m_commonBaseline, ch.bearing.y);
        m_commonHeight = std::max(m_commonHeight, ch.size.y);
    }
    if (m_commonHeight == 0)
        m_commonHeight = 128; // fallback

    // Done with FreeType.
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // 3. Create and compile the shader.
    m_shader = std::make_unique<Shader>(vertexShader, fragmentShader, geometryShader);
    m_shader->LinkUBO("GlobalUniforms", 0);

    // 4. Generate two VAOs and VBOs.
    glGenVertexArrays(2, m_VAO);
    glGenBuffers(2, m_VBO);

    // (a) VAO[0]: Label billboard vertex data.
    glBindVertexArray(m_VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
    // (We will upload the LabelData later in PrepareLabels.)
    // Layout: location 0 -> aCoord (vec3), location 1 -> aWidth (float), location 2 -> aTexIndex (float)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LabelData), (void *)offsetof(LabelData, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(LabelData), (void *)offsetof(LabelData, width));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(LabelData), (void *)offsetof(LabelData, texIndex));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // (b) VAO[1]: A simple quad for the geometry shader (unchanged from your original code).
    glBindVertexArray(m_VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 5. Build the text atlas from the label strings.
    CreateTextAtlas(labels);

    // 6. Prepare the billboard data (position, width, and texture layer index) for each label.
    PrepareLabels(labels);
}

LabelEngine::~LabelEngine() {
    glDeleteBuffers(2, m_VBO);
    glDeleteVertexArrays(2, m_VAO);
    glDeleteTextures(1, &m_textAtlas);
    // (Optionally, delete individual glyph textures if you are not reusing them elsewhere.)
}

void LabelEngine::RenderLabels(const glm::mat4 &view, const float time) {
    m_shader->Use();
    m_shader->SetFloat("time", time);
    m_shader->SetFloat("vHeight", 0.2f); // The uniform vertical half-size

    // Bind the text atlas texture array (assumed on texture unit 0)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textAtlas);
    m_shader->SetInt("textAtlas", 0);
    m_shader->SetVec3("textColor", glm::vec3(1.0f, 1.0f, 1.0f));

    glBindVertexArray(m_VAO[0]);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_activeLabels.size()));
    glBindVertexArray(0);
}

void LabelEngine::CreateTextAtlas(const std::vector<Label> &labels) {
    // First pass: determine each label’s width in pixels and the maximum width (for atlas layer size).
    int numLabels = static_cast<int>(labels.size());
    m_atlasLayerCount = numLabels;
    std::vector<int> labelWidths(numLabels, 0);
    int maxWidth = 0;
    for (int i = 0; i < numLabels; i++) {
        const std::string &text = labels[i].text;
        int width = 0;
        for (char c : text) {
            if (c < 0 || c >= 128)
                continue;
            // Note: advance.x is in 1/64 pixels.
            width += m_characters[c].advance / 64;
        }
        labelWidths[i] = width;
        maxWidth = std::max(maxWidth, width);
    }
    if (maxWidth <= 0)
        maxWidth = 1;

    m_atlasWidth = maxWidth;
    m_atlasHeight = m_commonHeight; // we use the same height for all layers

    // Create a texture array with (maxWidth x m_commonHeight) per layer.
    glGenTextures(1, &m_textAtlas);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textAtlas);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, m_atlasWidth, m_atlasHeight, numLabels, 0, GL_RED, GL_UNSIGNED_BYTE,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // For each label, create an image (filled with 0) and “draw” the text into it.
    for (int i = 0; i < numLabels; i++) {
        const std::string &text = labels[i].text;
        int textWidth = labelWidths[i];
        // Create a buffer (size = atlas layer dimensions) and clear to 0.
        std::vector<unsigned char> pixels(m_atlasWidth * m_atlasHeight, 0);
        int penX = 0;
        // Use the common baseline for vertical alignment.
        int baseline = m_commonBaseline;
        // For each character, copy its bitmap into the correct location.
        for (char c : text) {
            if (c < 0 || c >= 128)
                continue;
            const LabelCharacter &ch = m_characters[c];
            int xpos = penX + ch.bearing.x;
            int ypos = baseline - ch.bearing.y;
            // Loop over the glyph’s bitmap and copy pixels.
            for (int row = 0; row < ch.size.y; row++) {
                for (int col = 0; col < ch.size.x; col++) {
                    int x = xpos + col;
                    int y = ypos + row;
                    if (x < 0 || x >= m_atlasWidth || y < 0 || y >= m_atlasHeight)
                        continue;
                    // Copy the pixel value from the glyph’s saved bitmap.
                    pixels[y * m_atlasWidth + x] = ch.bitmapBuffer[row * ch.size.x + col];
                }
            }
            penX += ch.advance / 64;
        }
        // Upload this image into layer 'i' of the texture array.
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, m_atlasWidth, m_atlasHeight, 1, GL_RED, GL_UNSIGNED_BYTE,
                        pixels.data());
    }
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void LabelEngine::PrepareLabels(const std::vector<Label> &labels) {
    int numLabels = static_cast<int>(labels.size());
    m_activeLabels.resize(numLabels);
    for (int i = 0; i < numLabels; i++) {
        m_activeLabels[i].position = labels[i].position;
        int textWidth = 0;
        for (char c : labels[i].text) {
            if (c < 0 || c >= 128)
                continue;
            textWidth += m_characters[c].advance / 64;
        }
        // Compute “half‐width” (vWidth attribute) so that the full quad width is:
        // 2 * vWidth * (scaled by distance) and preserves the text’s aspect ratio.
        // Since the geometry shader uses a fixed vertical half‐size of vHeight (here 0.2),
        // we set: vWidth = (textWidth / commonHeight) * 0.2.
        m_activeLabels[i].width = (static_cast<float>(textWidth) / static_cast<float>(m_commonHeight)) * 0.2f;
        m_activeLabels[i].texIndex = static_cast<float>(i); // this layer in the atlas
    }

    // Upload the label billboard vertex data.
    glBindVertexArray(m_VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, m_activeLabels.size() * sizeof(LabelData), m_activeLabels.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
