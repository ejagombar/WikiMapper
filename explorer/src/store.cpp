#include "store.h"

void generateFakeData(DB &db, const int size) {
    const int idOffset = 1000;
    db.resize(size);

    NodeStore obj;
    for (int id = 0; id < size; ++id) {
        obj.UID = id + idOffset;
        obj.name = "Node" + std::to_string(obj.UID);

        int numOfLinks = 1 + rand() % 4;
        obj.linksTo.resize(numOfLinks);

        for (int i = 0; i < numOfLinks; i++) {
            obj.linksTo[i] = idOffset + rand() % size;
        }

        db[id] = obj;
    }
}
