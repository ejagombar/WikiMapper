#include <ostream>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <cstring>
// #include <fstream>
#include <iostream>

#include "saxparser.h"

int main(int argc, char *argv[]) {

    int return_code = EXIT_SUCCESS;

    if (argc <= 1) {
        std::cerr << "No input file provided" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filepath = argv[1];

    // Parse document in one go
    try {
        MySaxParser parser;
        parser.set_substitute_entities(true);
        parser.parse_file(filepath);

    } catch (const xmlpp::exception &ex) {
        std::cerr << "libxml++ exception: " << ex.what() << std::endl;
        return_code = EXIT_FAILURE;
    }

    return return_code;
}
