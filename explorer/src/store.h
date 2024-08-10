#ifndef STORE_H
#define STORE_H

#include <cstdint>
#include <string>
#include <vector>

struct NodeStore {
    uint32_t UID;
    std::string name;
    std::vector<uint32_t> linksTo;
};

typedef std::vector<NodeStore> DB;

void generateFakeData(DB &db, const int size);
#endif
