#ifndef CONTROLS_H
#define CONTROLS_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
  public:
    Camera(){};
    void computeMatricesFromInputs(GLFWwindow *window);
    glm::mat4 getViewMatrix() { return ViewMatrix; }
    glm::mat4 getProjectionMatrix() { return ProjectionMatrix; }

  private:
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;

    // Initial position : on +Z
    glm::vec3 position = glm::vec3(0, 0, 5);
    glm::vec3 acceleration = glm::vec3(0, 0, 0);
    // Initial horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
    // Initial vertical angle : none
    float verticalAngle = 0.0f;
    // Initial Field of View
    float initialFoV = 45.0f;

    float speed = 2.0f; // 3 units / second
    float maxSpeed = 40.0f;
    float mouseSpeed = 0.001f;
};

#endif
