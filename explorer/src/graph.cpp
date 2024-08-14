#include "graph.h"
#include <iostream>

using GraphDB::Graph;
using GraphDB::Node;

void Graph::addNode(uint32_t uid, const char *title) { nodes.emplace(uid, Node(uid, title)); }

void Graph::addEdge(uint32_t uid1, uint32_t uid2) {
    if (nodes.find(uid1) != nodes.end() && nodes.find(uid2) != nodes.end()) {
        adjList[uid1].push_back(uid2);
        adjList[uid2].push_back(uid1); // Assuming undirected graph
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

std::list<uint32_t> Graph::getNeighbors(uint32_t uid) const {
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
