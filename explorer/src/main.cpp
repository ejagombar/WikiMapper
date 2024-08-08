#include "neo4j.h"
#include <httplib.h>

#include "gui.h"
#include <json/json.h>

int main() {
    gui myGUI(1000000);
    return myGUI.init();
}
