#include "neo4j.h"
#include <cmath>
#include <cstdint>
#include <httplib.h>

#include "gui.h"
#include "store.h"
#include <iostream>
#include <json/json.h>
#include <unordered_map>

std::vector<glm::vec3> spreadOrbit2d(glm::vec3 center, const float xyPlainMin,
                                     const float xyPlainMax, const unsigned int count,
                                     const float radius) {
    std::vector<glm::vec3> out(count);

    const float angleDelta = ((xyPlainMax - xyPlainMin) / count);

    for (int i = 0; i < count; ++i) {
        auto &p = out[i];
        float angle = xyPlainMin + angleDelta * i;

        p.x = center.x + radius * cos(angle);
        p.y = center.y + radius * sin(angle);
    }

    return out;
}

std::vector<glm::vec3> spreadOrbit(glm::vec3 center, const float xyPlainMin, const float xyPlainMax,
                                   const float zPlainMin, const float zPlainMax, const int count,
                                   const double radius) {
    std::vector<glm::vec3> out(count);
    auto phi = 3.1415 * (sqrt(5.0) - 1);

    for (int i = 0; i < count; ++i) {
        double z = 1.0 - ((double)i / ((double)count - 1.0)) * 2.0;
        double radius2 = sqrt(1.0 - z * z);

        double theta = phi * i;

        out[i].x = (double)center.x + cos(theta) * radius2 * radius;
        out[i].y = (double)center.y + sin(theta) * radius2 * radius;
        out[i].z = (double)center.z + z * radius;
    }

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

    // std::vector<glm::vec3> out =
    //     spreadOrbit2d(glm::vec3(0, 0, 0), 0, 3.14159 * 2, numOfElements - 1, 100);
    std::vector<glm::vec3> out =
        spreadOrbit(glm::vec3(0, 0, 0), 0, 3.14159 * 2, 0, 0, numOfElements - 1, 100);

    out.insert(out.begin(), glm::vec3(0, 0, 0));

    const int size = 100;
    int i = 0;
    for (auto node : data) {
        // auto coord = glm::vec3((rand() % size - size / 2), (rand() % size - size / 2),
        //                        (rand() % size - size / 2));
        spaceMap.insert({node.UID, out[i]});

        std::cout << node.UID << " Coords: " << out[i].x << " " << out[i].y << " " << out[i].z
                  << " " << node.linksTo[0] << std::endl;
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

    std::cout << "Node Count: " << nodes.size() << std::endl;
    std::cout << "Last Node: " << nodes[nodes.size() - 1].size << std::endl;

    std::cout << "Coords: " << out[i].x << " " << out[i].y << " " << out[i].z << std::endl;

    for (auto startNode : data) {
        for (uint32_t endUID : startNode.linksTo) {
            lines.push_back(spaceMap[startNode.UID]);
            lines.push_back(spaceMap[endUID]);
        }
    }

    gui myGUI(numOfElements, lines, nodes);
    return myGUI.init();
}
