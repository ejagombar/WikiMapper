#include "neo4j.h"
#include <httplib.h>

#include "gui.h"
#include <json/json.h>

int main() {
    std::vector<glm::vec3> b;
    std::vector<Node> NodeContainer(100000);

    int size = 1000;
    for (int i = 0; i < 100000; i++) {
        NodeContainer[i].pos = glm::vec3((rand() % size - size / 2), (rand() % size - size / 2),
                                         (rand() % size - size / 2));

        NodeContainer[i].r = rand() % 256;
        NodeContainer[i].g = rand() % 256;
        NodeContainer[i].b = rand() % 256;
        NodeContainer[i].a = 255;

        NodeContainer[i].size = 1.0f;
    }

    int numLines = 2000; // Number of lines
    size = 1000;
    for (int i = 0; i < numLines; ++i) {

        glm::vec3 start = glm::vec3((rand() % size - size / 2), (rand() % size - size / 2),
                                    (rand() % size - size / 2));

        glm::vec3 end = glm::vec3((rand() % size - size / 2), (rand() % size - size / 2),
                                  (rand() % size - size / 2));

        b.push_back(start);
        b.push_back(end);
    }

    gui myGUI(100000, b, NodeContainer);
    return myGUI.init();
}
