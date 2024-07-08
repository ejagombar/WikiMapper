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

std::atomic<bool> processThread = true;
std::atomic<bool> writerThread = true;

std::atomic<int> processedPageCount = 0;
std::atomic<bool> stopOutputThread = false;
std::atomic<std::chrono::time_point<std::chrono::system_clock>> startTime;

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
                CSVFileNodes << "\"" << page.title << "\"" << std::flush;
                processedPageCount++;
            }
        }
    }

    CSVFileLinks.close();
    CSVFileNodes.close();
}

void OutputPageCount() {
    const int totalPages = 23603280;

    std::cout << "---------Info---------\n\n-------Loading--------\n\n" << std::endl;

    while (!stopOutputThread) {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        int count = processedPageCount.load();

        auto start = startTime.load();
        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;

        auto remainingSeconds = ((elapsed_seconds / count) * totalPages - elapsed_seconds).count();
        int hoursLeft = static_cast<int>(remainingSeconds) / 3600;
        int minutesLeft = (static_cast<int>(remainingSeconds) % 3600) / 60;
        int secondsLeft = static_cast<int>(remainingSeconds) % 60;

        int hoursTaken = static_cast<int>(elapsed_seconds.count()) / 3600;
        int minutesTaken = (static_cast<int>(elapsed_seconds.count()) % 3600) / 60;
        int secondsTaken = static_cast<int>(elapsed_seconds.count()) % 60;

        float percentageDone = (static_cast<float>(count) / totalPages) * 100.0;

        std::cout << std::setprecision(3) << std::fixed << "\r" << cursup << cursup << cursup << cursup
                  << "Page Number: " << count << "            \nProgress: " << percentageDone
                  << "%           \nTime Left: " << hoursLeft << " hrs " << minutesLeft << " mins " << secondsLeft
                  << " secs         \nTime Taken: " << hoursTaken << " hrs " << minutesTaken << " mins " << secondsTaken
                  << " secs         \n"

                  << std::flush;
    }
    std::cout << "Done!\a" << std::endl;
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
        for (size_t i = 0; i < 16; ++i) {
            processorThreads.emplace_back(pageProcessor, std::ref(qIn), std::ref(qOut));
        }

        std::thread writeThread(writer, std::ref(qOut));

        std::thread statsThread(OutputPageCount);

        startTime = std::chrono::system_clock::now();

        xmlpp::TextReader reader(filepath);
        while (reader.read()) {
            std::string name = reader.get_name();

            if (name == "page") {
                std::string output(reader.read_outer_xml());

                qIn.push(output);
            }
        }

        processThread = false;

        for (auto &t : processorThreads) {
            t.join();
        }

        writerThread = false;

        writeThread.join();

        stopOutputThread = true;

    } catch (const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
