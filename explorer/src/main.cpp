#include "store.hpp"
#include <ostream>
#include <tuple>
#define STB_IMAGE_IMPLEMENTATION
#include "./visual/engine.hpp"

#include "../lib/rgb_hsv.hpp"
#include "graph.hpp"
#include "pointMaths.hpp"

#include <cmath>
#include <cstdint>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <iostream>
#include <json/json.h>
#include <random>
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

float packRGBToFloat(unsigned char r, unsigned char g, unsigned char b) {
    uint32_t packed = (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | (static_cast<uint32_t>(b));
    return *reinterpret_cast<float *>(&packed);
}

void unpackFloatToRGB(float packedFloat, unsigned char &r, unsigned char &g, unsigned char &b) {
    uint32_t packed = *reinterpret_cast<uint32_t *>(&packedFloat);

    r = (packed >> 16) & 0xFF;
    g = (packed >> 8) & 0xFF;
    b = packed & 0xFF;
}

void generateRealData(GraphDB::Graph &graph) {
    Neo4jInterface neo4jDB("http://127.0.0.1:7474");
    if (!neo4jDB.Authenticate("neo4j", "test1234")) {
        return;
    }

    auto randomPage = neo4jDB.GetRandomPages(1).at(0);
    auto linkedPages = neo4jDB.GetLinkedPages("angus stone");

    int idx = 0;
    graph.addNode(0, "DOPE LEMON");

    for (const auto &page : linkedPages) {
        idx++;
        graph.addNode(idx, page.title.c_str());
        graph.addEdge(0, idx);
    }
    std::cout << idx << std::endl;
}

int main() {
    GraphDB::Graph db;
    generateRealData(db);

    auto allNodes = db.getAllNodes();
    const int numOfElements = allNodes.size();

    std::unordered_map<uint32_t, std::pair<glm::vec3, float>> spaceMap(numOfElements);

    // Display base node -----------------
    uint32_t baseNodeUID = getTopNode(db, allNodes);
    auto baseNode = db.getNode(baseNodeUID);

    spaceMap.insert({baseNodeUID, {glm::vec3(0, 0, 0), packRGBToFloat(50, 10, 200)}});

    auto neighboursUID = db.getNeighborsUID(baseNodeUID);
    auto out =
        spreadOrbit(spaceMap[baseNode->UID].first, neighboursUID.size(), 2 * sqrt(numOfElements), glm::vec3(0, 0, 0));

    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_real_distribution<> dist{0, 1};
    for (int i = 0; i < neighboursUID.size(); i++) {
        auto col = hsv2rgb(dist(gen), 1.0f, 1.0f);
        auto colFloat = packRGBToFloat(col.r, col.g, col.b);
        spaceMap.insert({neighboursUID[i], {out[i], colFloat}});
    }

    // Display next node -----------------
    auto neighbours = db.getNeighbors(baseNodeUID);

    uint32_t subNodeUID = getTopNode(db, neighbours);

    auto subNeighboursUID = db.getNeighborsUID(subNodeUID);

    glm::vec3 rotation = spaceMap[subNodeUID].first + glm::vec3(0, glm::pi<float>() * 0.5f, 0);
    auto subOut = spreadOrbitRand(spaceMap[subNodeUID].first, subNeighboursUID.size(),
                                  2 * sqrt(subNeighboursUID.size()), glm::vec2(1, 2), glm::vec2(1, 2), rotation);

    for (int i = 0; i < subNeighboursUID.size(); i++) {
        auto col = hsv2rgb(dist(gen), 1.0f, 1.0f);
        auto colFloat = packRGBToFloat(col.r, col.g, col.b);
        spaceMap.insert({subNeighboursUID[i], {subOut[i], colFloat}});
    }

    // -----------------------------------

    std::vector<Node> nodes(numOfElements);
    std::vector<Edge> edges;

    for (int i = 0; i < numOfElements; i++) {
        std::cout << i << std::endl;
        auto it = spaceMap.find(allNodes[i].UID);

        if (it != spaceMap.end()) {
            unsigned char r, g, b;

            unpackFloatToRGB(it->second.second, r, g, b);

            nodes[i].rgb[0] = r;
            nodes[i].rgb[1] = g;
            nodes[i].rgb[2] = b;

            nodes[i].text = allNodes[i].title;

            nodes[i].pos = it->second.first;
            nodes[i].size = 20;
        }
    }

    auto allLinks = db.getAllUniqueLinks();
    for (const auto linkPair : allLinks) {
        auto itStart = spaceMap.find(linkPair.first);
        auto itEnd = spaceMap.find(linkPair.second);

        if (itStart != spaceMap.end() and itEnd != spaceMap.end()) {
            Edge edge;

            edge.start = itStart->second.first;
            edge.end = itEnd->second.first;

            unsigned char r, g, b;
            unpackFloatToRGB(itStart->second.second, r, g, b);
            edge.startRGB[0] = r;
            edge.startRGB[1] = g;
            edge.startRGB[2] = b;

            unpackFloatToRGB(itEnd->second.second, r, g, b);
            edge.endRGB[0] = r;
            edge.endRGB[1] = g;
            edge.endRGB[2] = b;

            edge.size = 5;
            edges.push_back(edge);
        }
    }

    Engine myGUI(numOfElements, nodes, edges);
    myGUI.Run();
}
