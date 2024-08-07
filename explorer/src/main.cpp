#include "neo4j.h"
#include <httplib.h>

#include "gui.h"
#include <json/json.h>

int main() {
    // I do not like using a template for this. Maybe I should just declare the arrays on the heap.
    gui<100000> myGUI;
    return myGUI.init();
}
