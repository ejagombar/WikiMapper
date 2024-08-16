// ----------------------------------------
// Code from http://www.opengl-tutorial.org
// ----------------------------------------
#ifndef CONTROLS_H
#define CONTROLS_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
  public:
    Camera(){};
    void computeMatricesFromInputs(GLFWwindow *window);
    glm::mat4 getViewMatrix() { return m_viewMatrix; }
    glm::mat4 getProjectionMatrix() { return m_projectionMatrix; }

  private:
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    float m_horizontalAngle = 3.14f;
    float m_verticalAngle = 0.0f;
    float m_initialFoV = 45.0f;

    glm::vec3 m_position = glm::vec3(0, 0, 0);
    glm::vec3 m_acceleration = glm::vec3(0, 0, 0);

    const float m_movementSpeed = 2.0f;
    const float m_mouseSpeed = 0.001f;
    const float m_accelerationReduce = 0.99f;
    const float m_accelerationReduceSlowed = 0.95f;

    const float m_aspecRatio = 4.0f / 3.0f;
    const float m_minDisplayRange = 0.7f;
    const float m_maxDisplayRange = 1000.0f;

    const bool m_lockViewingAngle = false;
};

#endif
