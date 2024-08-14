#include "graph.h"
#include "gui.h"
#include "neo4j.h"
#include "pointMaths.h"
#include "store.h"

#include <cmath>
#include <cstdint>
#include <glm/detail/qualifier.hpp>
#include <httplib.h>
#include <iostream>
#include <json/json.h>
#include <unordered_map>

int main() {
    DB data;
    GraphDB::Graph test;
    const int numOfElements = 401;

    generateFakeData(data, numOfElements);
    test.addEdge(5, 5);

    std::vector<glm::vec3> lines;
    std::vector<Node> nodes(numOfElements);
    std::unordered_map<uint32_t, glm::vec3> spaceMap(numOfElements);

    auto DBSort = [](NodeStore a, NodeStore b) { return a.linksTo.size() > b.linksTo.size(); };
    std::sort(data.begin(), data.end(), DBSort);

    int numPoints = 400;
    float radius = 100.0f;

    auto out = spreadOrbitRand(glm::vec3(0, 0, 0), numPoints, radius, glm::vec3(1, 0, 0));

    out.push_back(rotateVec(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0)));

    int i = 0;
    std::cout << " Coords: " << data[400].UID << " " << std::endl;
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
    }

    std::cout << "Last Node: " << nodes[nodes.size() - 1].size << std::endl;

    for (auto startNode : data) {
        for (uint32_t endUID : startNode.linksTo) {
            lines.push_back(spaceMap[startNode.UID]);
            lines.push_back(spaceMap[endUID]);
        }
    }

    gui myGUI(numOfElements, lines, nodes);
    return myGUI.init();
}
