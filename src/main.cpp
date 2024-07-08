#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TSQueue.h"
#include "progress.h"
#include "saxparser.h"
#include <cstdlib>
#include <iostream>
#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>
#include <stdexcept>
#include <thread>

std::atomic<bool> processThread = true;
std::atomic<bool> writerThread = true;

void pageProcessor(TSQueue<std::string> &qIn, TSQueue<Page> &qOut) {
    while (processThread || !qIn.empty()) {
        std::string input = qIn.pop();
        MySaxParser parser;

        parser.set_substitute_entities(true);
        parser.parse_chunk(input);
        Page output = parser.GetPage();

        qOut.push(output);
    }
}

void writer(TSQueue<Page> &qIn) {
    std::remove("links.csv");
    std::remove("nodes.csv");

    std::ofstream CSVFileLinks;
    std::ofstream CSVFileNodes;

    CSVFileLinks.open("links.csv");
    CSVFileNodes.open("nodes.csv");

    while (writerThread || !qIn.empty()) {

        if (!qIn.empty()) {

            Page page = qIn.pop();
            if (page.title.size() > 0) {
                std::string outputstr = "";

                for (auto x : page.links) {
                    outputstr = outputstr + "\"" + page.title + "\",\"" + x + "\",LINK\n";
                }
                CSVFileLinks << outputstr << std::flush;
                CSVFileNodes << "\"" << page.title << "\"\n" << std::flush;
            }
        }
    }

    CSVFileLinks.close();
    CSVFileNodes.close();
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

        std::vector<std::thread> processorThreads;
        for (size_t i = 0; i < 14; ++i) {
            processorThreads.emplace_back(pageProcessor, std::ref(qIn), std::ref(qOut));
        }

        std::thread writeThread(writer, std::ref(qOut));

        Progress progress(23603280);

        xmlpp::TextReader reader(filepath);
        while (reader.read()) {
            std::string name = reader.get_name();

            if (name == "page") {
                std::string output(reader.read_outer_xml());

                qIn.push(output);
                progress.increment();
            }
            while (qIn.size() > 200) {
            }
        }

        processThread = false;

        for (auto &t : processorThreads) {
            t.join();
        }

        writerThread = false;

        writeThread.join();

    } catch (const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
