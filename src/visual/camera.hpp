#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN, SNEAK };

struct CameraPositionData {
    glm::vec3 position;
    float yaw;
    float pitch;
};

struct CameraMatrices {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec4 position;
};

class Camera {
  public:
    Camera();

    void SetPosition(const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), const float yaw = glm::pi<float>(),
                     const float pitch = 0.0f);

    void ProcessMovement(const float deltaTime);
    void ProcessKeyboard(const CameraMovement direction);
    void SetFov(const float fov) { m_fov = glm::clamp(fov, 1.0f, 160.0f); }
    void ProcessMouseMovement(const double xoffset, const double yoffset);
    void SetAspectRatio(const float aspectRatio) { m_aspectRatio = aspectRatio; }
    void SetMouseSensitivity(const float sensitivity) { m_mouseSensitivity = sensitivity * 0.001f; }

    const CameraPositionData GetPositionData() const { return CameraPositionData{m_position, m_yaw, m_pitch}; }
    const glm::mat4 &GetViewMatrix() const { return m_viewMatrix; }
    const glm::mat4 &GetProjectionMatrix() const { return m_projectionMatrix; }
    const glm::vec3 &GetCameraPosition() const { return m_position; }
    const glm::vec3 &GetVelocity() const { return m_velocity; }

    glm::mat3 CalcNormalMatrix() const;

    void SetThrustForce(float force) { m_thrustForce = force; }
    void SetLinearDamping(float damping) { m_linearDamping = damping; }
    void SetMaxVelocity(float maxVel) { m_maxVelocity = maxVel; }

  private:
    const glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_position;
    glm::vec3 m_velocity;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    float m_yaw;
    float m_pitch;
    float m_mouseSensitivity;
    float m_aspectRatio;
    float m_fov;

    float m_thrustForce = 300.0f;
    float m_linearDamping = 3.0f;
    float m_maxVelocity = 600.0f;

    glm::vec3 m_inputForce = glm::vec3(0.0f);
    float m_sneakMultiplier = 1.f;

    static constexpr float m_minDisplayRange = 0.1f;
    static constexpr float m_maxDisplayRange = 1000.0f;

    void updateCameraVectors();

    void updateMatrices();
};
