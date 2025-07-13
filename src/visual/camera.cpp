#include "camera.hpp"
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>

Camera::Camera()
    : m_position(0.0f), m_velocity(0.0f), m_yaw(glm::pi<float>()), m_pitch(0.0f), m_mouseSensitivity(0.0015f),
      m_aspectRatio(1.0f), m_fov(45.0f) {
    updateCameraVectors();
}

void Camera::SetPosition(const glm::vec3 position, const float yaw, const float pitch) {
    m_position = position;
    m_yaw = yaw;
    m_pitch = pitch;
    updateCameraVectors();
}

void Camera::ProcessKeyboard(const CameraMovement direction) {
    switch (direction) {
    case CameraMovement::FORWARD:
        m_inputForce += m_front;
        break;
    case CameraMovement::BACKWARD:
        m_inputForce -= m_front;
        break;
    case CameraMovement::LEFT:
        m_inputForce -= m_right;
        break;
    case CameraMovement::RIGHT:
        m_inputForce += m_right;
        break;
    case CameraMovement::UP:
        m_inputForce += m_worldUp;
        break;
    case CameraMovement::DOWN:
        m_inputForce -= m_worldUp;
        break;
    case CameraMovement::SNEAK:
        m_sneakMultiplier = 0.33f;
        break;
    }
}

void Camera::ProcessMouseMovement(const double xoffset, const double yoffset) {
    float xoff = static_cast<float>(xoffset) * m_mouseSensitivity;
    float yoff = static_cast<float>(yoffset) * m_mouseSensitivity;

    m_yaw += xoff;
    m_pitch = glm::clamp(m_pitch + yoff, glm::radians(-89.0f), glm::radians(89.0f));

    updateCameraVectors();
}

void Camera::ProcessMovement(const float deltaTime) {
    float clampedDeltaTime = glm::clamp(deltaTime, 0.0f, 1.0f / 20.0f);

    if (glm::length(m_inputForce) > 0.0f) {
        m_inputForce = glm::normalize(m_inputForce);
    }

    glm::vec3 totalAcceleration = m_inputForce * m_thrustForce * m_sneakMultiplier;
    glm::vec3 dampingForce = -m_velocity * m_linearDamping;
    totalAcceleration += dampingForce;

    m_velocity += totalAcceleration * clampedDeltaTime;
    float speed = glm::length(m_velocity);
    if (speed > m_maxVelocity) {
        m_velocity = (m_velocity / speed) * m_maxVelocity;
    }

    m_position += m_velocity * clampedDeltaTime;

    updateMatrices();

    m_inputForce = glm::vec3(0.0f);
    m_sneakMultiplier = 1.0f;
}

glm::mat3 Camera::CalcNormalMatrix() const {
    glm::mat3 submv = glm::mat3(m_viewMatrix);
    submv = glm::inverse(submv);
    return glm::transpose(submv);
}

void Camera::updateCameraVectors() {
    glm::vec3 direction(cos(m_yaw) * cos(m_pitch), sin(m_pitch), sin(m_yaw) * cos(m_pitch));

    m_front = glm::normalize(direction);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::updateMatrices() {
    m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_minDisplayRange, m_maxDisplayRange);

    m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
}
