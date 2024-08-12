#include "neo4j.h"
#include <cmath>
#include <cstdint>
#include <httplib.h>

#include "gui.h"
#include "store.h"
#include <iostream>
#include <json/json.h>
#include <unordered_map>

glm::vec3 rotateVec(glm::vec3 &input, glm::vec3 &rotation) {
    const glm::mat4 rotationMat(1.0f);

    glm::mat4 rotationMatX = glm::rotate(rotationMat, rotation.x, glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 rotationMatY = glm::rotate(rotationMat, rotation.y, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 rotationMatZ = glm::rotate(rotationMat, rotation.z, glm::vec3(0.0, 0.0, 1.0));

    return glm::vec3(rotationMatX * rotationMatY * rotationMatZ * glm::vec4(input, 1.0));
}

std::vector<glm::vec3> spreadOrbit2d(glm::vec3 &center, glm::vec3 &rotation, const float xyPlainMin,
                                     const float xyPlainMax, const unsigned int count,
                                     const float radius) {
    std::vector<glm::vec3> out(count);
    const float angleDelta = ((xyPlainMax - xyPlainMin) / count);

    for (int i = 0; i < count; ++i) {
        float angle = xyPlainMin + angleDelta * i;

        glm::vec3 location(center.x + radius * cos(angle), center.y + radius * sin(angle),
                           center.z);

        out[i] = rotateVec(location, rotation);
    }

    return out;
}

std::vector<glm::vec3> spreadOrbit(glm::vec3 center, const float xyPlainMin, const float xyPlainMax,
                                   const float zPlainMin, const float zPlainMax, const int count,
                                   const float radius) {
    std::vector<glm::vec3> out(count);
    const float pi = 3.14159274101257324219;
    const float phi = pi * (sqrt(5.0) - 1);

    for (int i = 0; i < count; ++i) {
        float z = 1.0 - (static_cast<float>(i) / (count - 1.0)) * 2;
        float unitRadius = sqrt(1.0 - z * z);

        float theta = phi * i;

        out[i].x = center.x + cos(theta) * unitRadius * radius;
        out[i].y = center.y + sin(theta) * unitRadius * radius;
        out[i].z = center.z + z * radius;
    }

    return out;
}

int main() {
    DB data;
    const int numOfElements = 26;

    generateFakeData(data, numOfElements);

    std::vector<glm::vec3> lines;
    std::vector<Node> nodes(numOfElements);
    std::unordered_map<uint32_t, glm::vec3> spaceMap(numOfElements);

    auto DBSort = [](NodeStore a, NodeStore b) { return a.linksTo.size() > b.linksTo.size(); };
    std::sort(data.begin(), data.end(), DBSort);

    auto out = spreadOrbit2d(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 1, 2, 10, 100);
    // auto out2 = spreadOrbit2d(glm::vec3(0, 0, 0), glm::vec3(3.14 * 2, 0, 0), 0, 3.14159, 49,
    // 100);

    out.push_back(glm::vec3(0, 0, 0));
    out.push_back(glm::vec3(50, 0, 0));
    out.push_back(glm::vec3(0, 50, 0));
    out.push_back(glm::vec3(50, 50, 0));
    auto x = 3.14f / 2.0f;
    out.push_back(rotateVec(glm::vec3(50, 0, 0), glm::vec3(x, 0, 0)));
    out.push_back(rotateVec(glm::vec3(50, 50, 0), glm::vec3(x, 0, 0)));
    out.push_back(rotateVec(glm::vec3(0, 50, 0), glm::vec3(x, 0, 0)));
    out.push_back(rotateVec(glm::vec3(0, 0, 0), glm::vec3(x, 0, 0)));

    out.push_back(rotateVec(glm::vec3(50, 0, 0), glm::vec3(0, 0, x)));
    out.push_back(rotateVec(glm::vec3(50, 50, 0), glm::vec3(0, 0, x)));
    out.push_back(rotateVec(glm::vec3(0, 50, 0), glm::vec3(0, 0, x)));
    out.push_back(rotateVec(glm::vec3(0, 0, 0), glm::vec3(0, 0, x)));

    out.push_back(rotateVec(glm::vec3(50, 0, 0), glm::vec3(0, x, 0)));
    out.push_back(rotateVec(glm::vec3(50, 50, 0), glm::vec3(0, x, 0)));
    out.push_back(rotateVec(glm::vec3(0, 50, 0), glm::vec3(0, x, 0)));
    out.push_back(rotateVec(glm::vec3(0, 0, 0), glm::vec3(0, x, 0)));

    // out.insert(out.end(), out2.begin(), out2.end());

    const int size = 100;
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
