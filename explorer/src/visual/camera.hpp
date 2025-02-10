#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN, SNEAK };

struct CameraPositionData {
    glm::vec3 position;
    GLfloat yaw;
    GLfloat pitch;
};

class Camera {
  public:
    Camera()
        : m_position(0.0f), m_acceleration(0.0f), m_velocity(0.0f), m_direction(0.0f), m_cameraPosition(0.0f),
          m_yaw(glm::pi<float>()), m_pitch(0.0f), m_mouseSensitivity(0.0015f), m_fov(45.0f), m_movementSpeed(1.5f),
          m_accelerationReduce(0.97f), m_aspectRatio(1.0f) {}

    void SetPosition(const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), const float yaw = glm::pi<float>(),
                     const float pitch = 0.0f);
    void SetAspectRatio(const float aspectRatio) { m_aspectRatio = aspectRatio; }

    inline const CameraPositionData GetPositionData() const;
    const glm::mat4 &GetViewMatrix() const { return m_viewMatrix; }
    const glm::mat4 &GetProjectionMatrix() const { return m_projectionMatrix; }
    const glm::vec3 &GetCameraPosition() const { return m_cameraPosition; }

    glm::mat3 CalcNormalMatrix() const;

    void ProcessKeyboard(const CameraMovement direction);
    void ProcessMouseMovement(const double xoffsetIn, const double yoffsetIn);
    void ProcessMouseScroll(const float yoffset);
    void ProcessPosition(const float deltaTime);

  private:
    const glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0);

    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_position;
    glm::vec3 m_acceleration;
    glm::vec3 m_velocity;
    glm::vec3 m_direction;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    glm::vec3 m_cameraPosition;

    GLfloat m_yaw;
    GLfloat m_pitch;
    GLfloat m_mouseSensitivity;
    GLfloat m_aspectRatio;
    GLfloat m_fov;

    GLfloat m_movementSpeed;
    GLfloat m_accelerationReduce;

    static constexpr GLfloat m_scrollSensitivity = 2.0f;
    static constexpr GLfloat m_minDisplayRange = 0.1f;
    static constexpr GLfloat m_maxDisplayRange = 1000.0f;

    void updateCameraVectors();
};

#endif
