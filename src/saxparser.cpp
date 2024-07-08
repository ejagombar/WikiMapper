#include "saxparser.h"
#include <cstdlib>
#include <ostream>
#include <pstl/glue_algorithm_defs.h>

MySaxParser::MySaxParser() : xmlpp::SaxParser() {}

MySaxParser::~MySaxParser() {}

void MySaxParser::on_start_document() {}

void MySaxParser::on_comment(const xmlpp::ustring &text) {}

void MySaxParser::on_warning(const xmlpp::ustring &text) {}

Page MySaxParser::GetPage() { return pages; }

void MySaxParser::on_end_document() {}

void MySaxParser::on_start_element(const xmlpp::ustring &name, const AttributeList &attributes) {
    if (name == "title") {
        nextElement = TITLE;
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

    if (depth == 1) {
        ExtractAllLinks();

        if (!page.redirect && !RE2::PartialMatch(page.title, "^.+:")) {
            std::string outputstr = "";
            for (auto x : page.links) {
                outputstr = outputstr + "\"" + page.title + "\",\"" + x + "\",LINK\n";
            }
            pages = page;
        }

        page = {};
        content = "";
    }

    depth--;
}

void MySaxParser::on_characters(const xmlpp::ustring &text) {
    if (nextElement == TITLE) {
        std::string title = text;
        FormatLink(title);

        page.title = page.title + title;

    } else if (nextElement == CONTENT) {
        content = content + text;
    }
}

void MySaxParser::FormatLink(std::string &str) {
    // Convert to lower case
    // transform(str.begin(), str.end(), str.begin(), ::tolower);

    // Replace double quotes with single quotes
    std::replace(str.begin(), str.end(), '\"', '\'');

    // Remove quotes from string
    // str.erase(remove(str.begin(), str.end(), '\"'), str.end());
}

void MySaxParser::on_error(const xmlpp::ustring &text) {
    std::cout << ". Most recent page title: " << page.title << ". Error: " << text << std::endl;
}

void MySaxParser::on_fatal_error(const xmlpp::ustring &text) {
    std::cout << ". Most recent page title: " << page.title << ". Error: " << text << std::endl;
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

        // Sometimes the page that is being linked to is not the same as the
        // text being shown in the link. This ensures that only the true page
        // name is shown
        auto x = find(str.begin(), str.end(), '|');
        auto subStr = std::string(str.begin(), x);

        FormatLink(subStr);

        page.links.push_back(subStr);
    }
}
