#include "neo4j.h"
#include <httplib.h>

#include <json/json.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow *window;

#include <glm/glm.hpp>
using namespace glm;

// int main() {
//     Neo4jData dbData;
//     dbData.auth64 = "bmVvNGo6dGVzdDEyMzQ=";
//     dbData.dbName = "neo4j";
//     dbData.host = "localhost";
//     dbData.port = 7474;
//
//     Neo4j db(dbData);
//
//     std::string start = "blues";
//     std::string end = "guitar";
//
//     auto res = db.shortestPath(start, end, 25);
//
//     if (res && res->status == 200) {
//         // std::cout << "Response status: " << res->status << std::endl;
//         // std::cout << "Response body: " << res->body << std::endl;
//
//         std::cout << res->body << std::endl;
//     } else {
//         std::cerr << "Request failed with status: " << (res ? res->status : 0) << std::endl;
//         if (res) {
//             std::cerr << "Error: " << res->body << std::endl;
//         }
//     }
//
//     return 0;
// }

int main(void) {
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                   GL_TRUE); // To make macOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "Playground", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 "
                        "compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    do {
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw nothing, see you in tutorial 2 !

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
