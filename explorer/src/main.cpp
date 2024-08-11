#include "neo4j.h"
#include <cstdint>
#include <httplib.h>

#include "gui.h"
#include "store.h"
#include <iostream>
#include <json/json.h>
#include <unordered_map>

std::vector<glm::vec3> spreadOrbit(glm::vec3 center, const float xyPlainMin, const float xyPlainMax,
                                   const float zPlainMin, const float zPlainMax,
                                   const unsigned int count, const float radius) {
    std::vector<glm::vec3> out;

    return out;
}

int main() {
    DB data;
    const int numOfElements = 100;

    generateFakeData(data, numOfElements);

    std::vector<glm::vec3> lines;
    std::vector<Node> nodes(numOfElements);
    std::unordered_map<uint32_t, glm::vec3> spaceMap(numOfElements);

    auto DBSort = [](NodeStore a, NodeStore b) { return a.linksTo.size() > b.linksTo.size(); };
    std::sort(data.begin(), data.end(), DBSort);

    auto out = spreadOrbit(glm::vec3(0, 0, 0), 0, 3.14, 0, 0, 1, 10);
    std::cout << "Coords: " << out.begin()->x << " " << out.begin()->y << " " << out.begin()->z
              << std::endl;

    const int size = 100;
    for (auto node : data) {
        auto coord = glm::vec3((rand() % size - size / 2), (rand() % size - size / 2),
                               (rand() % size - size / 2));
        spaceMap.insert({node.UID, coord});
    }

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
            lines.push_back(spaceMap[startNode.UID]);
            lines.push_back(spaceMap[endUID]);
        }
    }

    gui myGUI(numOfElements, lines, nodes);
    return myGUI.init();
}
