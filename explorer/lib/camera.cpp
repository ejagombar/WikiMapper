#include "camera.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

void Camera::computeMatricesFromInputs(GLFWwindow *window) {

    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Reset mouse position for next frame
    glfwSetCursorPos(window, 1024 / 2.0f, 768 / 2.0f);

    // Compute new orientation
    horizontalAngle += mouseSpeed * float(1024 / 2.0f - xpos);
    verticalAngle += mouseSpeed * float(768 / 2.0f - ypos);
    if (verticalAngle > 3.14 / 2)
        verticalAngle = 3.14 / 2;
    if (verticalAngle < -3.14 / 2)
        verticalAngle = -3.14 / 2;

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle),
                        cos(verticalAngle) * cos(horizontalAngle));

    // Right vector
    glm::vec3 right =
        glm::vec3(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f));

    // Up vector
    glm::vec3 up = glm::cross(right, direction);

    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        acceleration += direction;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        acceleration -= direction;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        acceleration += right;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        acceleration -= right;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        acceleration += glm::vec3(0, 1, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        acceleration -= glm::vec3(0, 1, 0);
    }
    position += acceleration * deltaTime * speed;
    acceleration *= 0.99;

    float FoV = initialFoV; // - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a
                            // callback for this. It's a bit too complicated for this beginner's
                            // tutorial, so it's disabled instead.

    // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.7f, 900.0f);
    // Camera matrix
    ViewMatrix =
        glm::lookAt(position,             // Camera is here
                    position + direction, // and looks here : at the same position, plus "direction"
                    up                    // Head is up (set to 0,-1,0 to look upside-down)
        );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}
