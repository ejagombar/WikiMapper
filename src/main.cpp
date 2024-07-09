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

void pageProcessor(TSQueue<std::string> &qIn, TSQueue<std::vector<Page>> &qOut) {

    MySaxParser parser;
    while (processThread || !qIn.empty()) {
        std::string input = qIn.pop();

        // parser.set_substitute_entities(true);
        parser.parse_memory(input);
        std::vector<Page> output = parser.GetPages();
        parser.ClearData();

        qOut.push(output);
    }
}

void writer(TSQueue<std::vector<Page>> &qIn) {
    std::remove("links.csv");
    std::remove("nodes.csv");

    std::ofstream CSVFileLinks;
    std::ofstream CSVFileNodes;

    CSVFileLinks.open("links.csv");
    CSVFileNodes.open("nodes.csv");

    CSVFileNodes << "pageName:ID" << std::endl;
    CSVFileLinks << ":START_ID,:END_ID,:TYPE" << std::endl;

    Progress progress(23603280);

    while (writerThread || !qIn.empty()) {
        if (qIn.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
            continue;
        }

        std::vector<Page> output = qIn.pop();
        for (Page page : output) {
            if (page.title.size() == 0) {
                continue;
            }

            std::string outputstr = "";
            // std::cout << qIn.size() << std::endl;

            for (auto x : page.links) {
                outputstr = outputstr + "\"" + page.title + "\",\"" + x + "\",LINK\n";
            }

            CSVFileLinks << outputstr << std::flush;
            CSVFileNodes << "\"" << page.title << "\"\n" << std::flush;
            progress.increment();
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
        TSQueue<std::vector<Page>> qOut;

        std::vector<std::thread> processorThreads;
        for (size_t i = 0; i < 16; ++i) {
            processorThreads.emplace_back(pageProcessor, std::ref(qIn), std::ref(qOut));
        }

        std::thread writeThread(writer, std::ref(qOut));

        xmlpp::TextReader reader(filepath);
        int pageCount = 0;
        std::string output = "<mediawiki>";
        while (reader.read()) {
            std::string name = reader.get_name();

            if (name == "page") {
                output += reader.read_outer_xml();
                pageCount++;

                if (pageCount >= 400) {
                    output += "\n</mediawiki>";
                    qIn.push(output);
                    output = "<mediawiki>\n";
                    pageCount = 0;
                }
            }

            while (qIn.size() > 100) {
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
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
