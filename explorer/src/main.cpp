#include "gui.h"
#include "neo4j.h"
#include "pointMaths.h"
#include "store.h"

#include <cmath>
#include <cstdint>
#include <httplib.h>
#include <iostream>
#include <json/json.h>
#include <unordered_map>

int main() {
    DB data;
    const int numOfElements = 4001;

    generateFakeData(data, numOfElements);

    std::vector<glm::vec3> lines;
    std::vector<Node> nodes(numOfElements);
    std::unordered_map<uint32_t, glm::vec3> spaceMap(numOfElements);

    auto DBSort = [](NodeStore a, NodeStore b) { return a.linksTo.size() > b.linksTo.size(); };
    std::sort(data.begin(), data.end(), DBSort);

    // auto out = spreadOrbit2d(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 0.5, 2.5, 20, 100);
    // auto x = 3.14f / 2.0f;
    // auto out2 = spreadOrbit2d(glm::vec3(100, 0, 0), glm::vec3(x, 2 / 2.0f, 0), 0.5, 2.5, 20,
    // 100);

    int numPoints = 4000;
    float radius = 100.0f;
    glm::vec2 thetaRange(glm::pi<float>() * -2.0f, glm::pi<float>() * 2.0f);
    glm::vec2 phiRange(0.0f, 500.0f);

    // Generate points
    // auto out = randomSpreadOrbit(numPoints, radius, thetaMin, thetaMax, phiMin, phiMax);
    auto out = generateFibonacciSphericalSector(numPoints, radius, thetaRange, phiRange);

    out.push_back(rotateVec(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0)));
    // out.insert(out.end(), out2.begin(), out2.end());

    int i = 0;
    std::cout << " Coords: " << data[i].UID << " " << std::endl;
    for (auto node : data) {
        spaceMap.insert({node.UID, out[i]});

        ++i;
    }

    for (int i = 0; i < numOfElements; i++) {
        nodes[i].pos = spaceMap[data[i].UID];

        nodes[i].r = rand() % 256;
        nodes[i].g = rand() % 256;
        nodes[i].b = rand() % 256;
        nodes[i].a = 255;

        nodes[i].size = data[i].linksTo.size();
        nodes[i].size = 1;
    }

    std::cout << "Node Count: " << nodes.size() << std::endl;
    std::cout << "Last Node: " << nodes[nodes.size() - 1].size << std::endl;

    std::cout << "Coords: " << out[i].x << " " << out[i].y << " " << out[i].z << std::endl;

    // for (auto startNode : data) {
    //     for (uint32_t endUID : startNode.linksTo) {
    //         lines.push_back(spaceMap[startNode.UID]);
    //         lines.push_back(spaceMap[endUID]);
    //     }
    // }

    gui myGUI(numOfElements, lines, nodes);
    return myGUI.init();
}
