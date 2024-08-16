// ----------------------------------------
// Code from http://www.opengl-tutorial.org
// ----------------------------------------
#include "camera.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Camera::computeMatricesFromInputs(GLFWwindow *window) {
    static double lastTime = glfwGetTime();

    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glfwSetCursorPos(window, width / 2.0f, height / 2.0f);

    m_horizontalAngle += m_mouseSpeed * float(width / 2.0f - xpos);
    m_verticalAngle += m_mouseSpeed * float(height / 2.0f - ypos);

    if (m_lockViewingAngle) {
        if (m_verticalAngle > 3.14 / 2)
            m_verticalAngle = 3.14 / 2;
        if (m_verticalAngle < -3.14 / 2)
            m_verticalAngle = -3.14 / 2;
    }

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(cos(m_verticalAngle) * sin(m_horizontalAngle), sin(m_verticalAngle),
                        cos(m_verticalAngle) * cos(m_horizontalAngle));

    glm::vec3 right =
        glm::vec3(sin(m_horizontalAngle - 3.14f / 2.0f), 0, cos(m_horizontalAngle - 3.14f / 2.0f));

    glm::vec3 up = glm::cross(right, direction);

    const glm::vec3 y(0, 1, 0);

    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_acceleration += glm::normalize(direction);
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_acceleration -= glm::normalize(direction);
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_acceleration += glm::normalize(right);
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_acceleration -= glm::normalize(right);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (m_lockViewingAngle)
            m_acceleration += y;
        else
            m_acceleration += glm::normalize(up);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (m_lockViewingAngle)
            m_acceleration -= y;
        else
            m_acceleration -= glm::normalize(up);
    }

    m_position += m_acceleration * deltaTime * m_movementSpeed;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        m_acceleration *= m_accelerationReduceSlowed;
    } else {
        m_acceleration *= m_accelerationReduce;
    }

    float FoV = m_initialFoV; // - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a
                              // callback for this. It's a bit too complicated for this beginner's
                              // tutorial, so it's disabled instead.

    m_projectionMatrix =
        glm::perspective(glm::radians(FoV), m_aspecRatio, m_minDisplayRange, m_maxDisplayRange);

    // Camera matrix
    m_viewMatrix = glm::lookAt(
        m_position,             // Camera is here
        m_position + direction, // and looks here : at the same position, plus "direction"
        up                      // Head is up (set to 0,-1,0 to look upside-down)
    );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}
