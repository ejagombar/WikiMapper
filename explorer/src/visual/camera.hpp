#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

struct CameraPositionData {
    glm::vec3 position;
    GLfloat yaw;
    GLfloat pitch;
};

class Camera {
  public:
    Camera() {};

    void SetPosition(const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), const float yaw = glm::pi<float>(),
                     const float pitch = 0.0f);

    CameraPositionData GetPosition() const;

    glm::mat4 GetViewMatrix() const { return m_viewMatrix; }
    glm::mat4 GetProjectionMatrix() const { return m_projectionMatrix; }
    glm::mat3 GetNormalMatrix() const;

    void ProcessKeyboard(const Camera_Movement direction);

    void ProcessMouseMovement(const double xoffsetIn, const double yoffsetIn);

    void ProcessMouseScroll(const float yoffset);

    void ProcessPosition(const float deltaTime);

    glm::vec3 GetCameraPosition() const { return m_cameraPosition; }

    void SetAspectRatio(const float aspectRatio) { m_aspectRatio = aspectRatio; }

  private:
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    const glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0);
    glm::vec3 m_position = glm::vec3(0, 0, 0);
    glm::vec3 m_acceleration = glm::vec3(0.1, 0.1, 0.1);
    glm::vec3 m_velocity = glm::vec3(0, 0, 0);
    glm::vec3 m_cameraPosition;
    glm::vec3 m_direction = glm::vec3(0, 0, 0);

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    GLfloat m_yaw = 3.14f;
    GLfloat m_pitch = 0.0f;
    GLfloat m_mouseSensitivity = 0.0015f;
    GLfloat m_aspectRatio;
    GLfloat m_fov = 45.0f;

    GLfloat m_movementSpeed = 1.0f;
    const GLfloat m_accelerationReduce = 0.97f;
    const GLfloat m_accelerationReduceSlowed = 0.95f;

    const GLfloat m_scrollSensitivity = 2.0f;
    const GLfloat m_minDisplayRange = 0.1f;
    const GLfloat m_maxDisplayRange = 500.0f;
    const bool m_lockViewingAngle = false;

    void updateCameraVectors();
};

#endif
