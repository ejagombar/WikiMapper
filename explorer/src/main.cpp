#include "neo4j.h"
#include <cstdint>
#include <httplib.h>

#include "gui.h"
#include "store.h"
#include <iostream>
#include <json/json.h>
#include <unordered_map>

int main() {
    DB data;
    const int numOfElements = 200;
    generateFakeData(data, numOfElements);
    std::unordered_map<uint32_t, glm::vec3> spaceMap(numOfElements);

    const int size = 200;
    std::cout << data.size() << std::endl;
    for (auto node : data) {
        auto coord = glm::vec3((rand() % size - size / 2), (rand() % size - size / 2),
                               (rand() % size - size / 2));
        spaceMap.insert({node.UID, coord});
    }

    std::vector<glm::vec3> lines;
    std::vector<Node> nodes(numOfElements);

    for (int i = 0; i < numOfElements; i++) {
        nodes[i].pos = spaceMap[data[i].UID];

        nodes[i].r = rand() % 256;
        nodes[i].g = rand() % 256;
        nodes[i].b = rand() % 256;
        nodes[i].a = 255;

        nodes[i].size = data[i].linksTo.size() / 1.0;
    }

    for (auto startNode : data) {
        for (uint32_t endUID : startNode.linksTo) {
            if (spaceMap.find(endUID) == spaceMap.end())
                std::cout << endUID << std::endl;
            lines.push_back(spaceMap[startNode.UID]);
            lines.push_back(spaceMap[endUID]);
        }
    }

    std::cout << "NODES " << nodes.size() << std::endl;
    for (auto x : nodes) {
        std::cout << x.pos.x << " " << x.pos.y << " " << x.pos.z << std::endl;
    }

    gui myGUI(numOfElements, lines, nodes);
    return myGUI.init();
}
