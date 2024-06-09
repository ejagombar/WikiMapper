#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <cstring>
// #include <fstream>
#include <iostream>

#include "myparser.h"

int main(int argc, char *argv[]) {
    std::string filepath;
    if (argc > 1)
        filepath = argv[1]; // Allow the user to specify a different XML file to
    else
        filepath = "example.xml";

    auto return_code = EXIT_SUCCESS;

    // Parse document in one go
    try {
        MySaxParser parser;
        parser.set_substitute_entities(true);
        parser.parse_file(filepath);

        auto pages = parser.GetPages();
        for (Page page : pages) {
            if (page.redirect)
                std::cout << "\nR---------" << page.title << "----------"
                          << std::endl;
            else
                std::cout << "\n----------" << page.title << "----------"
                          << std::endl;
            for (std::string s : page.links) {
                std::cout << s;
            }
        }
    } catch (const xmlpp::exception &ex) {
        std::cerr << "libxml++ exception: " << ex.what() << std::endl;
        return_code = EXIT_FAILURE;
    }

    // // Incremental parsing, sometimes useful for network connections:
    // try {
    //     std::cout << std::endl << "Incremental SAX Parser:" << std::endl;
    //
    //     std::ifstream is(filepath.c_str());
    //     if (!is)
    //         throw xmlpp::exception("Could not open file " + filepath);
    //
    //     char buffer[512];
    //     const size_t buffer_size = sizeof(buffer) / sizeof(char);
    //
    //     // Parse the file:
    //     MySaxParser parser;
    //     // xmlpp::SaxParser parser;
    //     parser.set_substitute_entities(true);
    //     do {
    //         std::memset(buffer, 0, buffer_size);
    //         is.read(buffer, buffer_size - 1);
    //         if (is.gcount()) {
    //             xmlpp::ustring input(buffer, buffer + is.gcount());
    //             parser.parse_chunk(input);
    //         }
    //     } while (is);
    //
    //     parser.finish_chunk_parsing();
    //
    // } catch (const xmlpp::exception &ex) {
    //     std::cerr << "Incremental parsing, libxml++ exception: " << ex.what()
    //               << std::endl;
    //     return_code = EXIT_FAILURE;
    // }

    return return_code;
}

// #ifdef HAVE_CONFIG_H
// #include <config.h>
// #endif
//
// #include <libxml++/libxml++.h>
// #include <libxml++/parsers/textreader.h>
//
// #include <cstdlib>
// #include <iostream>
//
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
//
// int main(int argc, char *argv[]) {
//     try {
//         std::string filepath;
//         if (argc > 1) {
//             filepath = argv[1];
//
//         } else {
//             filepath = "example.xml";
//         }
//         xmlpp::TextReader reader(filepath);
//
//         while (reader.read()) {
//             int depth = reader.get_depth();
//             std::string name = reader.get_name();
//
//             if (name == "page") {
//                 std::cout << indent(depth) << "--- node ---" << std::endl;
//                 std::cout << indent(depth) << "name: " << reader.get_name()
//                           << std::endl;
//                 std::cout << indent(depth) << "depth: " << reader.get_depth()
//                           << std::endl;
//
//                 std::cout << reader.read_inner_xml() << std::endl;
//                 reader.read();
//                 reader.read();
//                 std::cout << indent(depth) << "name: " << reader.get_name()
//                           << std::endl;
//                 // if (reader.has_attributes()) {
//                 //     std::cout << indent(depth) << "attributes: " <<
//                 //     std::endl; reader.move_to_first_attribute(); do {
//                 //         std::cout << indent(depth) << "  " <<
//                 //         reader.get_name()
//                 //                   << ": " << reader.get_value() <<
//                 std::endl;
//                 //     } while (reader.move_to_next_attribute());
//                 //     reader.move_to_element();
//                 // } else {
//                 //     std::cout << indent(depth) << "no attributes" <<
//                 //     std::endl;
//                 // }
//
//                 // if (reader.has_value())
//                 //     std::cout << indent(depth) << "value: '"
//                 //               << reader.get_value() << "'" << std::endl;
//                 // else
//                 //     std::cout << indent(depth) << "novalue" << std::endl;
//             }
//         }
//     } catch (const std::exception &e) {
//         std::cerr << "Exception caught: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
//     return EXIT_SUCCESS;
// }
