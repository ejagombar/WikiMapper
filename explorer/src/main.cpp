#define STB_IMAGE_IMPLEMENTATION
#include "./visual/engine.hpp"

#include "graph.hpp"
#include "pointMaths.hpp"

#include <cmath>
#include <cstdint>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <json/json.h>
#include <unordered_map>
#include <vector>

uint32_t getTopNode(GraphDB::Graph &db, std::vector<GraphDB::Node> &nodes) {
    int maxLinkCount(0);
    uint32_t topNode(0);
    for (auto n : nodes) {
        int linkCount = db.getNeighborsUID(n.UID).size();
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

    auto allNodes = db.getAllNodes();
    const int numOfElements = allNodes.size();

    std::vector<glm::vec3> lines;
    std::vector<Node> nodes(numOfElements);
    std::unordered_map<uint32_t, glm::vec3> spaceMap(numOfElements);

    // Display base node -----------------

    // std::cout << "UID: " << allNodes[0].title << std::endl;
    uint32_t baseNodeUID = getTopNode(db, allNodes);
    auto baseNode = db.getNode(baseNodeUID);

    spaceMap.insert({baseNodeUID, glm::vec3(0, 0, 0)});

    auto neighboursUID = db.getNeighborsUID(baseNodeUID);
    auto out = spreadOrbit(spaceMap[baseNode->UID], neighboursUID.size(), 2 * sqrt(numOfElements), glm::vec3(0, 0, 0));

    for (int i = 0; i < neighboursUID.size(); i++) {
        spaceMap.insert({neighboursUID[i], out[i]});
    }

    // Display next node -----------------
    auto neighbours = db.getNeighbors(baseNodeUID);

    uint32_t subNodeUID = getTopNode(db, neighbours);

    auto subNeighboursUID = db.getNeighborsUID(subNodeUID);
    // std::cout << " test" << subNeighboursUID.size() << std::endl;
    glm::vec3 rotation = spaceMap[subNodeUID] + glm::vec3(0, glm::pi<float>() * 0.5f, 0);
    auto subOut = spreadOrbitRand(spaceMap[subNodeUID], neighboursUID.size(), 2 * sqrt(subNeighboursUID.size()),
                                  glm::vec2(1, 2), glm::vec2(1, 2), rotation);

    for (int i = 0; i < subNeighboursUID.size(); i++) {
        spaceMap.insert({subNeighboursUID[i], subOut[i]});
    }

    // -----------------------------------

    for (int i = 0; i < numOfElements; i++) {
        auto it = spaceMap.find(allNodes[i].UID);

        nodes[i].r = rand() % 256;
        nodes[i].g = rand() % 256;
        nodes[i].b = rand() % 256;

        nodes[i].text = allNodes[i].title;

        if (it != spaceMap.end()) {
            nodes[i].pos = it->second;
            nodes[i].a = 255;
            nodes[i].size = 20;
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

    GUI myGUI(numOfElements, nodes, lines);
    myGUI.run();
}
