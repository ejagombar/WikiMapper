#include "./text.hpp"
#include <stdexcept>

Text::Text(const std::string fontPath, const std::string vertexShader, const std::string fragmentShader) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");
    }

    if (FT_New_Face(ft, fontPath.c_str(), 0, &m_face)) {
        throw std::runtime_error("ERROR::FREETYPE: Failed to load font");
    }

    FT_Set_Pixel_Sizes(m_face, 0, 128);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph
        if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) {
            throw std::runtime_error("ERROR::FREETYTPE: Failed to load Glyph");
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows, 0, GL_RED,
                     GL_UNSIGNED_BYTE, m_face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {texture, glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
                               glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
                               static_cast<uint>(m_face->glyph->advance.x)};

        m_characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(m_face);
    FT_Done_FreeType(ft);

    m_textShader = std::make_unique<Shader>(vertexShader.c_str(), fragmentShader.c_str());

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void Text::SetTransforms(const glm::mat4 projection, const glm::mat4 view, const glm::vec3 cameraPosition) {
    m_textShader->use();
    m_textShader->setMat4("projection", projection);
    m_textShader->setMat4("view", view);
    m_textShader->setVec3("CameraUp_worldspace", view[0][1], view[1][1], view[2][1]);
    m_textShader->setVec3("CameraRight_worldspace", view[0][0], view[1][0], view[2][0]);
    m_textShader->setVec3("CameraPos", cameraPosition);
}

void Text::Render(const std::string text, glm::vec3 position, const float scale, const glm::vec3 color) {
    m_textShader->use();
    m_textShader->setVec3("textColor", color.x, color.y, color.z);
    m_textShader->setVec3("BillboardPos", position);
    m_textShader->setFloat("BillboardSize", scale);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);

    // iterate through all characters
    std::string::const_iterator c;

    float totalWidth(0);
    Character ch;
    for (c = text.begin(); c != text.end(); c++) {
        ch = m_characters[*c];
        totalWidth += static_cast<float>(ch.Advance >> 6);
    }

    position.x = totalWidth * -0.5f;
    position.y = m_characters['o'].Bearing.y * -0.5f;

    for (c = text.begin(); c != text.end(); c++) {
        Character ch = m_characters[*c];

        float xpos = position.x + ch.Bearing.x;
        float ypos = position.y - (ch.Size.y - ch.Bearing.y);
        float zpos = 1.25f;

        float w = ch.Size.x;
        float h = ch.Size.y;
        // update VBO for each character
        float vertices[6][5] = {{xpos, ypos + h, zpos, 0.0f, 0.0f},    {xpos, ypos, zpos, 0.0f, 1.0f},
                                {xpos + w, ypos, zpos, 1.0f, 1.0f},

                                {xpos, ypos + h, zpos, 0.0f, 0.0f},    {xpos + w, ypos, zpos, 1.0f, 1.0f},
                                {xpos + w, ypos + h, zpos, 1.0f, 0.0f}};
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        position.x += (ch.Advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Text2d::UpdateScreenSize(const float width, const float height) {
    m_projection = glm::ortho(0.0f, width, 0.0f, height);
    SetTransforms(m_projection, glm::mat4(1.0f), glm::vec3(0, 0, 0));
}

void Text2d::Render2d(const std::string text, const glm::vec3 position, const float scale, const glm::vec3 color) {
    Render(text, position, scale, color);
}
