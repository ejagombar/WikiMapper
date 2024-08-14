#include "graph.h"
#include "gui.h"
#include "neo4j.h"
#include "pointMaths.h"

#include <cmath>
#include <cstdint>
#include <glm/detail/qualifier.hpp>
#include <httplib.h>
#include <iostream>
#include <json/json.h>
#include <unordered_map>

uint32_t getTopNode(GraphDB::Graph &db) {
    auto nodes = db.getAllNodes();
    int maxLinkCount(0);
    uint32_t topNode(0);
    for (auto n : nodes) {
        int linkCount = db.getNeighbors(n.UID).size();
        if (linkCount > maxLinkCount) {
            maxLinkCount = linkCount;
            topNode = n.UID;
        }
    }
    return topNode;
}

int main() {
    GraphDB::Graph db;
    generateFakeData(db);

    const int numOfElements = db.getAllNodes().size();

    std::vector<glm::vec3> lines;
    std::vector<Node> nodes(numOfElements);
    std::unordered_map<uint32_t, glm::vec3> spaceMap(numOfElements);

    uint32_t baseNodeUID = getTopNode(db);
    auto baseNode = db.getNode(baseNodeUID);

    spaceMap[baseNode->UID] = glm::vec3(0, 0, 0);

    auto neighbours = db.getNeighbors(baseNodeUID);
    auto out = spreadOrbit(spaceMap[baseNode->UID], neighbours.size(), sqrt(numOfElements),
                           glm::vec3(0, 0, 0));

    for (int i = 0; i < neighbours.size(); i++) {
        spaceMap.insert({neighbours[i], out[i]});
    }

    auto allNodes = db.getAllNodes();
    for (int i = 0; i < numOfElements; i++) {
        auto it = spaceMap.find(allNodes[i].UID);

        nodes[i].r = rand() % 256;
        nodes[i].g = rand() % 256;
        nodes[i].b = rand() % 256;

        if (it != spaceMap.end()) {
            nodes[i].pos = it->second;
            nodes[i].a = 255;
            nodes[i].size = 1;
        } else {
            nodes[i].pos = glm::vec3(50, 50, 50);
            nodes[i].a = 0;
            nodes[i].size = 0;
        }
    }

    auto allLinks = db.getAllLinks();
    for (auto linkPair : allLinks) {
        lines.push_back(spaceMap[linkPair.first]);
        lines.push_back(spaceMap[linkPair.second]);
    }

    gui myGUI(numOfElements, lines, nodes);
    return myGUI.init();
}
