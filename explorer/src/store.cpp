#include "store.hpp"
#include <random>
#include <vector>

void generateFakeData(DB &db, const int size) {

    const int idOffset = 1000;
    db.resize(size);

    std::default_random_engine generator;
    std::binomial_distribution<int> distribution(25, 0.1);

    NodeStore obj;
    for (int id = 0; id < size; ++id) {
        obj.UID = id + idOffset;
        obj.name = "Node" + std::to_string(obj.UID);

        // int numOfLinks = 1 + rand() % 4;
        int numOfLinks = distribution(generator) + 1;

        // obj.linksTo.resize(numOfLinks);
        obj.linksTo.resize(1);

        // for (int i = 0; i < numOfLinks; i++) {
        //     obj.linksTo[i] = idOffset + rand() % size;
        // }

        obj.linksTo[0] = 1126;

        db[id] = obj;
    }
}
