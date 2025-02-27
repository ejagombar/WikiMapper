#include "../lib/rgb_hsv.hpp"
#include "./visual/engine.hpp"
#include "graph.hpp"
#include "pointMaths.hpp"
#include "store.hpp"
#include <cmath>
#include <cstdint>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <json/json.h>
#include <random>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/std_image.h"

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
    auto linkedPages = neo4jDB.GetLinkedPages("mathematics");

    graph.AddNode("Mathematics");

    for (const auto &page : linkedPages) {
        const uint32_t idx = graph.AddNode(page.title.c_str());
        graph.AddEdge(0, idx);
    }
}

int main() {
    GraphDB::Graph db;
    generateRealData(db);

    const int numOfElements = db.m_nodes.size();

    // Display base node -----------------
    uint32_t baseNodeIdx = db.GetTopNode();
    auto baseNode = db.m_nodes[baseNodeIdx];
    std::cout << baseNode.title;

    auto neighboursUID = db.GetNeighboursIdx(baseNodeIdx);
    auto out =
        spreadOrbit(db.m_nodes[baseNodeIdx].pos, neighboursUID.size(), 2 * sqrt(numOfElements), glm::vec3(0, 0, 0));

    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_real_distribution<> dist{0, 1};
    for (int i = 0; i <= neighboursUID.size(); i++) {
        auto col = hsv2rgb(dist(gen), 0.8f, 1.0f);
        auto colFloat = packRGBToFloat(col.r, col.g, col.b);
        db.m_nodes[i].pos = out[i];
        db.m_nodes[i].colour = colFloat;
    }

    db.m_nodes[baseNodeIdx].pos = glm::vec3(0, 0, 0);

    std::vector<Node> nodes(numOfElements);
    std::vector<Edge> edges;

    for (int i = 0; i < numOfElements; i++) {
        unsigned char r, g, b;

        unpackFloatToRGB(db.m_nodes[i].colour, r, g, b);

        nodes[i].rgb[0] = r;
        nodes[i].rgb[1] = g;
        nodes[i].rgb[2] = b;

        nodes[i].text = db.m_nodes[i].title;

        nodes[i].pos = db.m_nodes[i].pos;
        nodes[i].size = 20;
    }

    for (const auto e : db.m_edges) {
        Edge edge;

        const GraphDB::Node &startNode = db.m_nodes[e.startIdx];
        const GraphDB::Node &endNode = db.m_nodes[e.endIdx];

        edge.start = startNode.pos;
        edge.end = endNode.pos;

        unsigned char r, g, b;
        unpackFloatToRGB(startNode.colour, r, g, b);
        edge.startRGB[0] = r;
        edge.startRGB[1] = g;
        edge.startRGB[2] = b;

        unpackFloatToRGB(endNode.colour, r, g, b);
        edge.endRGB[0] = r;
        edge.endRGB[1] = g;
        edge.endRGB[2] = b;

        edge.size = 5;
        edges.push_back(edge);
    }

    Engine myGUI(numOfElements, nodes, edges);
    myGUI.Run();
}
