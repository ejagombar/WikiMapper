#include <glad/glad.h>
#include <glm/glm.hpp>

template <typename T> class UBOManager {
  public:
    UBOManager(GLuint bindingPoint) : bindingPoint(bindingPoint) {
        static_assert(std::is_trivially_copyable<T>::value,
                      "UBOManager requires trivially copyable types for uniform data.");

        glGenBuffers(1, &uboID);
        glBindBuffer(GL_UNIFORM_BUFFER, uboID);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(T), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, uboID);
    }

    ~UBOManager() { glDeleteBuffers(1, &uboID); }

    void Update(const T &data) {
        glBindBuffer(GL_UNIFORM_BUFFER, uboID);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    GLuint GetID() const { return uboID; }
    GLuint GetBindingPoint() const { return bindingPoint; }

  private:
    GLuint uboID;
    GLuint bindingPoint;
};
