#include "camera.hpp"
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>

void Camera::SetPosition(const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), const float yaw = glm::pi<float>(),
                         const float pitch = 0.0f) {
    m_position = position;
    m_yaw = yaw;
    m_pitch = pitch;
    updateCameraVectors();
}

const CameraPositionData Camera::GetPositionData() const { return CameraPositionData{m_position, m_yaw, m_pitch}; }

void Camera::ProcessKeyboard(const CameraMovement movement) {
    if (movement == CameraMovement::FORWARD)
        m_direction += m_front;
    if (movement == CameraMovement::BACKWARD)
        m_direction -= m_front;
    if (movement == CameraMovement::LEFT)
        m_direction -= m_right;
    if (movement == CameraMovement::RIGHT)
        m_direction += m_right;
    if (movement == CameraMovement::UP)
        m_direction += m_worldUp;
    if (movement == CameraMovement::DOWN)
        m_direction -= m_worldUp;
    if (movement == CameraMovement::SNEAK)
        m_accelerationReduce = 0.92f;
    else
        m_accelerationReduce = 0.97f;
}

void Camera::ProcessMouseMovement(const double xoffsetIn, const double yoffsetIn) {
    float xoffset = static_cast<float>(xoffsetIn);
    float yoffset = static_cast<float>(yoffsetIn);

    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (m_pitch > glm::radians(89.0f))
        m_pitch = glm::radians(89.0f);
    if (m_pitch < glm::radians(-89.0f))
        m_pitch = glm::radians(-89.0f);

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(const float yoffset) {
    m_fov -= (float)yoffset * m_scrollSensitivity;
    if (m_fov < 1.0f)
        m_fov = 1.0f;
    if (m_fov > 160.0f)
        m_fov = 160.0f;
}

void Camera::ProcessPosition(const float deltaTime) {
    if (glm::length(m_direction) > 0.0f) {
        m_direction = glm::normalize(m_direction);
    }

    m_acceleration += m_direction;
    m_direction = glm::vec3(0, 0, 0);

    m_acceleration *= m_accelerationReduce;
    m_position += m_acceleration * deltaTime * m_movementSpeed;
    m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_minDisplayRange, m_maxDisplayRange);
    m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
    m_cameraPosition = (glm::inverse(m_viewMatrix)[3]);
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
