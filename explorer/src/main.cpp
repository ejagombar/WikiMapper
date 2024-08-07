#include "neo4j.h"
#include <httplib.h>

#include "gui.h"
#include <json/json.h>

int main() {
    gui myGUI(100000);
    return myGUI.init();
}
