#include "store.h"
#include <random>

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
        int numOfLinks = distribution(generator);

        obj.linksTo.resize(numOfLinks);

        for (int i = 0; i < numOfLinks; i++) {
            obj.linksTo[i] = idOffset + rand() % size;
        }

        db[id] = obj;
    }
}
