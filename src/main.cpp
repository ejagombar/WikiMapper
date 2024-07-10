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

void pageProcessor(TSQueue<std::string> &qIn, TSQueue<std::vector<Page>> &qOut, std::atomic<bool> &keepAlive) {
    MySaxParser parser;

    while (keepAlive.load() || !qIn.empty()) {
        parser.parse_memory(qIn.pop());
        qOut.push(parser.GetPages());
        parser.Clear();
    }
}

void csvWriter(TSQueue<std::vector<Page>> &qIn, std::ofstream &nodeFile, std::ofstream &linkFile,
               std::atomic<bool> &keepAlive) {
    Progress progress(23603280);

    std::string linkStr;
    while (keepAlive.load() || !qIn.empty()) {
        if (qIn.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
            continue;
        }

        for (Page page : qIn.pop()) {
            if (page.title.size() == 0) {
                continue;
            }

            linkStr = "";
            for (std::string x : page.links) {
                linkStr = linkStr + "\"" + page.title + "\",\"" + x + "\",LINK\n";
            }

            linkFile << linkStr << std::flush;
            nodeFile << "\"" << page.title << "\"" << std::endl;
            progress.increment();
        }
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

        std::ofstream CSVFileLinks;
        std::ofstream CSVFileNodes;

        TSQueue<std::string> qIn;
        TSQueue<std::vector<Page>> qOut;

        std::atomic<bool> processKeepAlive = true;
        std::atomic<bool> writerKeepAlive = true;

        std::remove("links.csv");
        std::remove("nodes.csv");

        CSVFileLinks.open("links.csv");
        CSVFileNodes.open("nodes.csv");

        CSVFileNodes << "pageName:ID" << std::endl;
        CSVFileLinks << ":START_ID,:END_ID,:TYPE" << std::endl;

        std::vector<std::thread> processorThreads;
        for (size_t i = 0; i < 16; ++i) {
            processorThreads.emplace_back(pageProcessor, std::ref(qIn), std::ref(qOut), std::ref(processKeepAlive));
        }

        std::thread writerThread(csvWriter, std::ref(qOut), std::ref(CSVFileNodes), std::ref(CSVFileLinks),
                                 std::ref(writerKeepAlive));

        int pageCount(0);
        std::string output("<mediawiki>");
        xmlpp::TextReader reader(filepath);
        while (reader.read()) {
            if (reader.get_name() == "page") {
                output += reader.read_outer_xml();
                pageCount++;

                if (pageCount >= 400) {
                    output += "\n</mediawiki>";
                    qIn.push(output);
                    output = "<mediawiki>\n";
                    pageCount = 0;
                }
            }

            while (qIn.size() > 50) {
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
            }
        }

        processKeepAlive = false;
        for (auto &t : processorThreads) {
            t.join();
        }

        writerKeepAlive = false;
        writerThread.join();

        CSVFileLinks.close();
        CSVFileNodes.close();

    } catch (const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
