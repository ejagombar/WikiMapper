#include <thread>
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

void pageProcessor(TSQueue<std::string> &qIn, TSQueue<Page> &qOut) {
    while (!stopProcessThread || !qIn.empty()) {
        std::string input = qIn.pop();
        MySaxParser parser;

        parser.set_substitute_entities(true);
        parser.parse_chunk(input);
        Page output = parser.GetPage();

        qOut.push(output);
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

        std::thread readerThread(pageProcessor, std::ref(qIn), std::ref(qOut));

        xmlpp::TextReader reader(filepath);
        while (reader.read()) {
            std::string name = reader.get_name();

            if (name == "page") {
                std::string output(reader.read_outer_xml());

                qIn.push(output);
            }
        }

        stopProcessThread = true;

        readerThread.join();

        std::cout << "Finished Loop" << std::endl;
        while (!qOut.empty()) {
            Page p = qOut.pop();
            if (true) {
                std::cout << p.title << " " << p.redirect << std::endl;
                std::vector<std::string> links = p.links;
                for (std::string l : links) {
                    std::cout << l << std::endl;
                }
            }
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
