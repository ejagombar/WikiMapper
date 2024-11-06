#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

class Camera {
  public:
    Camera() {};

    void SetPosition(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                     float yaw = glm::pi<float>(), float pitch = 0.0f);

    glm::mat4 GetViewMatrix() { return m_viewMatrix; }
    glm::mat4 GetProjectionMatrix() { return m_projectionMatrix; }

    void ProcessKeyboard(Camera_Movement direction);

    void ProcessMouseMovement(double xoffsetIn, double yoffsetIn);

    void ProcessMouseScroll(float yoffset);

    void ProcessPosition(float deltaTime);

    glm::vec3 GetCameraPosition() { return m_cameraPosition; }

    void SetAspectRatio(const float aspectRatio) { m_aspectRatio = aspectRatio; }

  private:
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    glm::vec3 m_position = glm::vec3(0, 0, 0);
    glm::vec3 m_acceleration = glm::vec3(0.1, 0.1, 0.1);
    glm::vec3 m_velocity = glm::vec3(0, 0, 0);
    glm::vec3 m_cameraPosition;
    glm::vec3 m_direction = glm::vec3(0, 0, 0);

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    float m_yaw = 3.14f;
    float m_pitch = 0.0f;
    float m_mouseSensitivity = 0.0015f;
    float m_aspectRatio;
    float m_fov = 45.0f;

    float m_movementSpeed = 1.0f;
    const float m_accelerationReduce = 0.97f;
    const float m_accelerationReduceSlowed = 0.95f;

    const float m_scrollSensitivity = 2.0f;
    const float m_minDisplayRange = 0.1f;
    const float m_maxDisplayRange = 500.0f;
    const bool m_lockViewingAngle = false;

    void updateCameraVectors();
};

#endif
