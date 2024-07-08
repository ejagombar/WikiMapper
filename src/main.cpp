#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TSQueue.h"
#include "saxparser.h"
#include <cstdlib>
#include <iostream>
#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>
#include <stdexcept>

std::atomic<bool> stopProcessThread = false;

void pageProcessor(TSQueue<std::string> qIn, TSQueue<Page> qOut) {
    while (!stopProcessThread) {
        std::string input = qIn.pop();
    }
}

int main(int argc, char *argv[]) {
    try {
        std::string filepath;

        if (argc > 1) {
            filepath = argv[1];
        } else {
            throw std::invalid_argument("No file provided");
        }

        TSQueue<std::string> qIn;
        TSQueue<Page> qOut;

        xmlpp::TextReader reader(filepath);
        while (reader.read()) {
            std::string name = reader.get_name();
            if (name == "page") {
                std::string output(reader.read_outer_xml());
                qIn.push(output);
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
