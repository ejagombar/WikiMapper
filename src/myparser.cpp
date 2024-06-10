#include "myparser.h"
#include <fstream>

MySaxParser::MySaxParser() : xmlpp::SaxParser() {
    std::thread(&MySaxParser::OutputPageCount, this).detach();
}

MySaxParser::~MySaxParser() {
    stopOutputThread = true;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void MySaxParser::OutputPageCount() {
    while (!stopOutputThread) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        int count = processedPageCount.load();
        float percentageDone = (static_cast<float>(count) / 3800000) * 100.0;
        std::cout << "Processed page count: " << count << " " << percentageDone
                  << "%" << std::endl;
    }
}

void MySaxParser::on_start_document() { CSVFile.open("out.csv"); }

void MySaxParser::on_comment(const xmlpp::ustring &text) {}

void MySaxParser::on_warning(const xmlpp::ustring &text) {}

std::vector<Page> MySaxParser::GetPages() { return pages; }

void MySaxParser::on_end_document() {
    std::cout << "Finished processing document. \nPage Count: "
              << processedPageCount << std::endl;
    CSVFile.close();
}

void MySaxParser::on_start_element(const xmlpp::ustring &name,
                                   const AttributeList &attributes) {
    if (name == "title") {
        nextElement = TITLE;
        processedPageCount++;
    } else if (name == "text") {
        nextElement = CONTENT;
    } else if (name == "redirect") {
        page.redirect = true;
    } else {
        nextElement = OTHER;
    }

    depth++;
}

void MySaxParser::on_end_element(const xmlpp::ustring & /* name */) {
    nextElement = OTHER;

    if (depth == 2) {

        ExtractAllLinks();

        // pages.push_back(page);
        if (!page.redirect) {
            for (auto x : page.links)
                CSVFile << page.title << ',' << x << ",links" << std::endl;
        }

        page = {};
        content = "";
    }

    depth--;
}

void MySaxParser::on_characters(const xmlpp::ustring &text) {
    if (nextElement == TITLE) {
        std::string lowerString = text;
        transform(lowerString.begin(), lowerString.end(), lowerString.begin(),
                  ::tolower);

        page.title = lowerString;
    } else if (nextElement == CONTENT) {
        content = content + text;
    }
}

void MySaxParser::on_error(const xmlpp::ustring &text) {
    std::cout << "ERROR on page " << processedPageCount
              << ". Most recent page title: " << page.title
              << ". Error: " << text << std::endl;
}

void MySaxParser::on_fatal_error(const xmlpp::ustring &text) {
    std::cout << "FATAL ERROR on page " << processedPageCount
              << ". Most recent page title: " << page.title
              << ". Error: " << text << std::endl;
}

void MySaxParser::ExtractAllLinks() {

    re2::RE2 re("\\[\\[([^\\]]+)\\]\\]");
    re2::StringPiece input(content);

    re2::StringPiece match;
    while (RE2::FindAndConsume(&input, re, &match)) {

        // Filters out any Wikipedia Namespaces
        // https://en.wikipedia.org/wiki/Wikipedia:Namespace
        if (RE2::PartialMatch(match, "^.+:")) {
            continue;
        }

        std::string str(match);
        transform(str.begin(), str.end(), str.begin(), ::tolower);

        // Sometimes the page that is being linked to is not the same as the
        // text being shown in the link. This ensures that only the true page
        // name is shown
        auto x = find(str.begin(), str.end(), '|');
        auto subStr = std::string(str.begin(), x);

        page.links.push_back(subStr);
    }
}
