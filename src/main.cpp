// #include <ostream>
// #ifdef HAVE_CONFIG_H
// #include <config.h>
// #endif
//
// #include <cstdlib>
// #include <cstring>
// #include <iostream>
//
// #include "saxparser.h"
//
// int main(int argc, char *argv[]) {
//
//     int return_code = EXIT_SUCCESS;
//
//     if (argc <= 1) {
//         std::cerr << "No input file provided" << std::endl;
//         return EXIT_FAILURE;
//     }
//
//     std::string filepath = argv[1];
//
//     try {
//         MySaxParser parser;
//         parser.set_substitute_entities(true);
//         parser.parse_file(filepath);
//
//     } catch (const xmlpp::exception &ex) {
//         std::cerr << "libxml++ exception: " << ex.what() << std::endl;
//         return_code = EXIT_FAILURE;
//     }
//
//     return return_code;
// }

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>

#include <cstdlib>
#include <iostream>

// struct indent {
//     int depth_;
//     explicit indent(int depth) : depth_(depth) {};
// };
//
// std::ostream &operator<<(std::ostream &o, indent const &in) {
//     for (int i = 0; i != in.depth_; ++i) {
//         o << "  ";
//     }
//     return o;
// }

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
            std::string name = reader.get_name();

            if (name == "page") {
                std::cout << "--- node ---" << std::endl;
                std::cout << "name: " << reader.get_name() << std::endl;
                std::cout << "depth: " << reader.get_depth() << std::endl;

                std::cout << reader.read_inner_xml() << std::endl;
                reader.read();
                reader.read();
                std::cout << "name: " << reader.get_name() << std::endl;
                if (reader.has_attributes()) {
                    std::cout << "attributes: " << std::endl;
                    reader.move_to_first_attribute();
                    do {
                        std::cout << "  " << reader.get_name() << ": " << reader.get_value() << std::endl;
                    } while (reader.move_to_next_attribute());
                    reader.move_to_element();
                } else {
                    std::cout << "no attributes" << std::endl;
                }

                if (reader.has_value())
                    std::cout << "value: '" << reader.get_value() << "'" << std::endl;
                else
                    std::cout << "novalue" << std::endl;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
