#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class Shader {
  public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath,
           const std::string &geometryPath = std::string());
    ~Shader();

    void Use();

    void SetBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, GLint value) const;
    void SetFloat(const std::string &name, GLfloat value) const;

    void SetVec2(const std::string &name, const glm::vec2 &value) const;
    void SetVec2(const std::string &name, GLfloat x, GLfloat y) const;
    void SetVec3(const std::string &name, const glm::vec3 &value) const;
    void SetVec3(const std::string &name, GLfloat x, GLfloat y, GLfloat z) const;
    void SetVec4(const std::string &name, const glm::vec4 &value) const;
    void SetVec4(const std::string &name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const;
    void SetMat2(const std::string &name, const glm::mat2 &mat) const;
    void SetMat3(const std::string &name, const glm::mat3 &mat) const;
    void SetMat4(const std::string &name, const glm::mat4 &mat) const;
    void LinkUBO(const std::string &blockName, GLuint bindingPoint) const;

    GLint GetAttribLocation(const std::string &name) const;

    GLuint ID;

  private:
    void checkCompileErrors(GLuint shader, std::string type);

    const std::string &m_vertexPath;
    const std::string &m_fragmentPath;
    const std::string &m_geometryPath;
};

#endif
