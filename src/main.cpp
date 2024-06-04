
// int main() {
//     std::cout << "Hello World" << std::endl;
//     std::cout << "Hello World 2" << std::endl;
//
//     // pugi::xml_document doc;
//     //
//     // pugi::xml_parse_result result = doc.load_file("tree.xml");
//     //
//     // std::cout << "Load result: " << result.description()
//     //           << ", mesh name: " <<
//     //           doc.child("mesh").attribute("name").value()
//     //           << std::endl;
//
//     char buffer_in[256] = {"Conan is a MIT-licensed, Open Source package "
//                            "manager for C and C++ development "
//                            "for C and C++ development, allowing development "
//                            "teams to easily and efficiently "
//                            "manage their packages and dependencies across "
//                            "platforms and build systems."};
//     char buffer_out[256] = {0};
//
//     z_stream defstream;
//     defstream.zalloc = Z_NULL;
//     defstream.zfree = Z_NULL;
//     defstream.opaque = Z_NULL;
//     defstream.avail_in = (uInt)strlen(buffer_in);
//     defstream.next_in = (Bytef *)buffer_in;
//     defstream.avail_out = (uInt)sizeof(buffer_out);
//     defstream.next_out = (Bytef *)buffer_out;
//
//     // deflateInit(&defstream, Z_BEST_COMPRESSION);
//     // deflate(&defstream, Z_FINISH);
//     // deflateEnd(&defstream);
//
//     printf("Uncompressed size is: %lu\n", strlen(buffer_in));
//     printf("Compressed size is: %lu\n", strlen(buffer_out));
//
//     return EXIT_SUCCESS;
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <cstring> // std::memset()
#include <fstream>
#include <iostream>

#include "myparser.h"

int main(int argc, char *argv[]) {
    std::string filepath;
    if (argc > 1)
        filepath =
            argv[1]; // Allow the user to specify a different XML file to parse.
    else
        filepath = "example.xml";

    // Parse the entire document in one go:
    auto return_code = EXIT_SUCCESS;
    try {
        MySaxParser parser;
        parser.set_substitute_entities(true);
        parser.parse_file(filepath);
    } catch (const xmlpp::exception &ex) {
        std::cerr << "libxml++ exception: " << ex.what() << std::endl;
        return_code = EXIT_FAILURE;
    }

    // Incremental parsing, sometimes useful for network connections:
    try {
        std::cout << std::endl << "Incremental SAX Parser:" << std::endl;

        std::ifstream is(filepath.c_str());
        if (!is)
            throw xmlpp::exception("Could not open file " + filepath);

        char buffer[64];
        const size_t buffer_size = sizeof(buffer) / sizeof(char);

        // Parse the file:
        MySaxParser parser;
        parser.set_substitute_entities(true);
        do {
            std::memset(buffer, 0, buffer_size);
            is.read(buffer, buffer_size - 1);
            if (is.gcount()) {
                xmlpp::ustring input(buffer, buffer + is.gcount());
                parser.parse_chunk(input);
            }
        } while (is);

        parser.finish_chunk_parsing();
    } catch (const xmlpp::exception &ex) {
        std::cerr << "Incremental parsing, libxml++ exception: " << ex.what()
                  << std::endl;
        return_code = EXIT_FAILURE;
    }

    return return_code;
}
/
}
