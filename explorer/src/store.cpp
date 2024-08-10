#include "store.h"

void generateFakeData(DB &db) {
    const int idOffset = 1000;
    const int size = 100;
    db.resize(size);

    NodeStore obj;
    for (int id = 0; id < size; ++id) {
        obj.UID = id + idOffset;
        obj.name = "Node" + std::to_string(obj.UID);

        int numOfLinks = 2 + rand() % 12;
        obj.linksTo.resize(numOfLinks);

        for (int i = 0; i < numOfLinks; i++) {
            obj.linksTo[i] = idOffset + rand() % (idOffset + size);
        }

        db[id] = obj;
    }
}
