#include "neo4j.h"
#include <httplib.h>

#include "gui.h"
#include "store.h"
#include <json/json.h>

int main() {
    DB data;
    generateFakeData(data);

    const int nodeCount = 100000;
    std::vector<glm::vec3> lines;
    std::vector<Node> nodes(nodeCount);

    const int size = 1000;
    for (int i = 0; i < 100000; i++) {
        nodes[i].pos = glm::vec3((rand() % size - size / 2), (rand() % size - size / 2),
                                 (rand() % size - size / 2));

        nodes[i].r = rand() % 256;
        nodes[i].g = rand() % 256;
        nodes[i].b = rand() % 256;
        nodes[i].a = 255;

        nodes[i].size = 1.0f;
    }

    int numLines = 4000;
    for (int i = 0; i < numLines; ++i) {

        glm::vec3 start = glm::vec3((rand() % size - size / 2), (rand() % size - size / 2),
                                    (rand() % size - size / 2));

        glm::vec3 end = glm::vec3((rand() % size - size / 2), (rand() % size - size / 2),
                                  (rand() % size - size / 2));

        lines.push_back(start);
        lines.push_back(end);
    }

    gui myGUI(nodeCount, lines, nodes);
    return myGUI.init();
}
