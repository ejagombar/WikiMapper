#include "graph.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

using GraphDB::Graph;
using GraphDB::Node;

void Graph::addNode(uint32_t uid, const char *title) { nodes.emplace(uid, Node(uid, title)); }

void Graph::addEdge(uint32_t uid1, uint32_t uid2) {
    if (nodes.find(uid1) != nodes.end() && nodes.find(uid2) != nodes.end()) {
        adjList[uid1].push_back(uid2);
    } else {
        std::cerr << "Error: One or both nodes not found." << std::endl;
    }
}

const Node *Graph::getNode(uint32_t uid) const {
    auto it = nodes.find(uid);
    if (it != nodes.end()) {
        return &(it->second);
    }
    return nullptr;
}

std::vector<Node> Graph::getAllNodes() const {
    std::vector<Node> allNodes;
    for (const auto &pair : nodes) {
        allNodes.push_back(pair.second);
    }
    return allNodes;
}

std::vector<std::pair<uint32_t, uint32_t>> Graph::getAllLinks() const {

    std::vector<std::pair<uint32_t, uint32_t>> allLinks;
    for (const auto pair : adjList) {
        for (const auto pairLink : pair.second) {
            // allLinks.emplace_back(pair.first, pair.second);
            allLinks.push_back({pair.first, pairLink});
        }
    }
    return allLinks;
}

std::vector<Node> Graph::getNeighbors(uint32_t uid) const {
    std::vector<uint32_t> UIDs;
    if (adjList.find(uid) != adjList.end()) {
        UIDs = adjList.at(uid);
    }

    std::vector<Node> out;
    out.reserve(UIDs.size());
    for (auto uid : UIDs) {
        out.push_back(nodes.find(uid)->second);
    }
    return out;
}

std::vector<uint32_t> Graph::getNeighborsUID(uint32_t uid) const {
    if (adjList.find(uid) != adjList.end()) {
        return adjList.at(uid);
    }
    return {};
}

void Graph::printGraph() const {
    for (const auto &pair : nodes) {
        const Node &node = pair.second;
        std::cout << "Node UID: " << node.UID << ", Title: " << node.title << std::endl;
        std::cout << "  Neighbors: ";
        if (adjList.find(node.UID) != adjList.end()) {
            for (const auto &neighbor : adjList.at(node.UID)) {
                std::cout << neighbor << " ";
            }
        }
        std::cout << std::endl;
    }
}

void generateFakeData(GraphDB::Graph &graph) {

    graph.addNode(1, "Music");
    graph.addNode(2, "Instruments");
    graph.addNode(3, "Blues");
    graph.addNode(4, "Jimmy Hendrix");
    graph.addNode(5, "Black Dog");
    graph.addNode(6, "Sheet Music");
    graph.addNode(7, "Paper");
    graph.addNode(8, "Time Signature");

    std::array<const char *, 25> instruments = {"Piano",       "Guitar",      "Violin",    "Drums",       "Flute",
                                                "Saxophone",   "Trumpet",     "Cello",     "Clarinet",    "Harp",
                                                "Trombone",    "Bass Guitar", "Oboe",      "Accordion",   "Banjo",
                                                "Mandolin",    "Ukulele",     "Tuba",      "French Horn", "Viola",
                                                "Synthesizer", "Marimba",     "Xylophone", "Harmonica",   "Bagpipes"};

    int index = 9;
    for (auto instrument : instruments) {
        graph.addNode(index, instrument);
        graph.addEdge(2, index);
        graph.addEdge(index, 2);
        index++;
    }

    std::array<const char *, 9> musicWords = {"Melody", "Rythm",     "Ryme", "Tempo", "Crotchet",
                                              "Quaver", "Signature", "ear",  "pitch"};

    index = 9 + 25 + 1;
    for (auto musicWord : musicWords) {
        graph.addNode(index, musicWord);
        graph.addEdge(1, index);
        graph.addEdge(index, 1);
        index++;
    }

    graph.addEdge(2, 1);
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(1, 6);
    graph.addEdge(1, 8);
    graph.addEdge(3, 4);
    graph.addEdge(4, 5);
    graph.addEdge(5, 4);
    graph.addEdge(6, 7);
    graph.addEdge(6, 8);
    graph.addEdge(8, 6);
}
