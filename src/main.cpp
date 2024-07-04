#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../lib/TSQueue.h"
#include <cstdlib>
#include <iostream>
#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>
#include <stdexcept>

int main(int argc, char *argv[]) {
    try {
        std::string filepath;

        if (argc > 1) {
            filepath = argv[1];
        } else {
            throw std::invalid_argument("No file provided");
        }

        TSQueue<int> q;

        q.push(5);
        xmlpp::TextReader reader(filepath);
        while (reader.read()) {
            std::string name = reader.get_name();
            if (name == "page") {
                std::cout << q.pop() << std::endl;
                std::string output(reader.read_outer_xml());
                std::cout << "!Name: " << output << std::endl;
            } else {
                std::cout << "Name: " << name << std::endl;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
