#include "graph.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <unordered_set>
#include <utility>
#include <vector>

using GraphDB::Graph;
using GraphDB::Node;

void Graph::AddNode(uint32_t uid, const char *title) { nodes.emplace(uid, Node(uid, title)); }

void Graph::AddEdge(uint32_t uid1, uint32_t uid2) {
    if (nodes.find(uid1) != nodes.end() && nodes.find(uid2) != nodes.end()) {
        adjList[uid1].push_back(uid2);
    } else {
        std::cerr << "Error: One or both nodes not found." << std::endl;
    }
}

const Node *Graph::GetNode(uint32_t uid) const {
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
            allLinks.push_back({pair.first, pairLink});
        }
    }
    return allLinks;
}
struct pair_hash {
    template <class T1, class T2> std::size_t operator()(const std::pair<T1, T2> &pair) const {
        return std::hash<T1>{}(pair.first) ^ (std::hash<T2>{}(pair.second) << 1);
    }
};
std::vector<std::pair<uint32_t, uint32_t>> Graph::getAllUniqueLinks() const {
    std::unordered_set<std::pair<uint32_t, uint32_t>, pair_hash> uniqueLinks;

    for (const auto &pair : adjList) {
        uint32_t uid1 = pair.first;
        for (const auto &uid2 : pair.second) {
            if (uid1 < uid2) {
                uniqueLinks.emplace(uid1, uid2);
            } else if (uid1 > uid2) {
                uniqueLinks.emplace(uid2, uid1);
            }
        }
    }

    // Convert the set back to a vector
    return std::vector<std::pair<uint32_t, uint32_t>>(uniqueLinks.begin(), uniqueLinks.end());
}

// Hash function for unordered_set of pairs

std::vector<Node> Graph::GetNeighbors(uint32_t uid) const {
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

std::vector<uint32_t> Graph::GetNeighborsUID(uint32_t uid) const {
    if (adjList.find(uid) != adjList.end()) {
        return adjList.at(uid);
    }
    return {};
}

void Graph::PrintGraph() const {
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

    graph.AddNode(1, "Music");
    graph.AddNode(2, "Instruments");
    graph.AddNode(3, "Blues");
    graph.AddNode(4, "Jimmy Hendrix");
    graph.AddNode(5, "Black Dog");
    graph.AddNode(6, "Sheet Music");
    graph.AddNode(7, "Paper");
    graph.AddNode(8, "Time Signature");

    std::array<const char *, 25> instruments = {"Piano",       "Guitar",      "Violin",    "Drums",       "Flute",
                                                "Saxophone",   "Trumpet",     "Cello",     "Clarinet",    "Harp",
                                                "Trombone",    "Bass Guitar", "Oboe",      "Accordion",   "Banjo",
                                                "Mandolin",    "Ukulele",     "Tuba",      "French Horn", "Viola",
                                                "Synthesizer", "Marimba",     "Xylophone", "Harmonica",   "Bagpipes"};

    int index = 9;
    for (auto instrument : instruments) {
        graph.AddNode(index, instrument);
        graph.AddEdge(2, index);
        graph.AddEdge(index, 2);
        index++;
    }

    std::array<const char *, 9> musicWords = {"Melody", "Rythm",     "Ryme", "Tempo", "Crotchet",
                                              "Quaver", "Signature", "ear",  "pitch"};

    index = 9 + 25 + 1;
    for (auto musicWord : musicWords) {
        graph.AddNode(index, musicWord);
        graph.AddEdge(1, index);
        graph.AddEdge(index, 1);
        index++;
    }

    graph.AddEdge(2, 1);
    graph.AddEdge(1, 2);
    graph.AddEdge(1, 3);
    graph.AddEdge(1, 6);
    graph.AddEdge(1, 8);
    graph.AddEdge(3, 4);
    graph.AddEdge(4, 5);
    graph.AddEdge(5, 4);
    graph.AddEdge(6, 7);
    graph.AddEdge(6, 8);
    graph.AddEdge(8, 6);
}
