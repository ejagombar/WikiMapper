#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <iostream>
#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>
#include <mutex>
#include <queue>

int main(int argc, char *argv[]) {
    try {
        std::string filepath;
        if (argc > 1) {
            filepath = argv[1];

        } else {
            filepath = "example.xml";
        }
        xmlpp::TextReader reader(filepath);
        while (reader.read()) {
            int depth = reader.get_depth();
            if (reader.get_name() == "page") {

                std::cout << "--- node ---" << std::endl;

                std::cout << reader.read_outer_xml() << std::endl;
            }
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
