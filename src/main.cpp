#include "../lib/pugixml/src/pugixml.hpp"
#include <iostream>

int main() {
    std::cout << "Hello World" << std::endl;
    std::cout << "Hello World 2" << std::endl;

    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file("tree.xml");

    std::cout << "Load result: " << result.description()
              << ", mesh name: " << doc.child("mesh").attribute("name").value()
              << std::endl;
}
