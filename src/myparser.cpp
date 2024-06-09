#include "myparser.h"

#include <iostream>
#include <ostream>

MySaxParser::MySaxParser() : xmlpp::SaxParser() {}

MySaxParser::~MySaxParser() {}

void MySaxParser::on_start_document() {
    // std::cout << "on_start_document()" << std::endl;
}

void MySaxParser::on_end_document() {
    // std::cout << "on_end_document()" << std::endl;
}

void MySaxParser::on_start_element(const xmlpp::ustring &name,
                                   const AttributeList &attributes) {
    // std::cout << "node name=" << name << std::endl;

    depth++;
    if (name == "title") {
        nextElement = TITLE;
    } else if (name == "text") {
        nextElement = CONTENT;
    } else if (name == "redirect") {
        page.redirect = true;
    } else {
        nextElement = OTHER;
    }
}

void MySaxParser::on_end_element(const xmlpp::ustring & /* name */) {
    // std::cout << "on_end_element() Depth: " << depth << std::endl;
    nextElement = OTHER;

    if (depth == 2) {

        // std::cout << content << std::endl;
        ExtractAllLinks();
        // pages.push_back(page);

        if (page.redirect)
            std::cout << "\n\n---------" << page.title
                      << "---------- (REDIRECT)" << std::endl;
        else
            std::cout << "\n\n----------" << page.title << "----------"
                      << std::endl;
        for (std::string s : page.links) {
            std::cout << s;
        }
        page = {};
        content = "";
    }

    depth--;
}

void MySaxParser::on_characters(const xmlpp::ustring &text) {
    if (nextElement == TITLE) {
        page.title = text;
    } else if (nextElement == CONTENT) {
        content = content + text;
    }
}

void MySaxParser::on_comment(const xmlpp::ustring &text) {
    // std::cout << "on_comment(): " << text << std::endl;
}

void MySaxParser::on_warning(const xmlpp::ustring &text) {
    // std::cout << "on_warning(): " << text << std::endl;
}

void MySaxParser::on_error(const xmlpp::ustring &text) {
    // std::cout << "on_error(): " << text << std::endl;
}

void MySaxParser::on_fatal_error(const xmlpp::ustring &text) {
    // std::cout << "on_fatal_error(): " << text << std::endl;
}

std::vector<Page> MySaxParser::GetPages() { return pages; }

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
