#include "myparser.h"

#include <iostream>
#include <ostream>

MySaxParser::MySaxParser() : xmlpp::SaxParser() {}

MySaxParser::~MySaxParser() {}

void MySaxParser::on_start_document() {
    std::cout << "on_start_document()" << std::endl;
}

void MySaxParser::on_end_document() {
    std::cout << "on_end_document()" << std::endl;
}

void MySaxParser::on_start_element(const xmlpp::ustring &name,
                                   const AttributeList &attributes) {
    std::cout << "node name=" << name << std::endl;

    depth++;
    if (name == "title") {
        nextElement = title;
    } else if (name == "text") {
        std::cout << "next elemeent is text" << std::endl;
        nextElement = content;
    } else {
        nextElement = other;
    }
}

void MySaxParser::on_end_element(const xmlpp::ustring & /* name */) {
    std::cout << "on_end_element() Depth: " << depth << std::endl;
    nextElement = other;

    if (depth == 2) {

        std::cout << page.content << std::endl;
        pages.push_back(page);
        page = {};
    }

    depth--;
}

void MySaxParser::on_characters(const xmlpp::ustring &text) {
    if (nextElement == title) {
        page.title = text;
    } else if (nextElement == content) {
        page.content = page.content + text;
    }
}

void MySaxParser::on_comment(const xmlpp::ustring &text) {
    std::cout << "on_comment(): " << text << std::endl;
}

void MySaxParser::on_warning(const xmlpp::ustring &text) {
    std::cout << "on_warning(): " << text << std::endl;
}

void MySaxParser::on_error(const xmlpp::ustring &text) {
    std::cout << "on_error(): " << text << std::endl;
}

void MySaxParser::on_fatal_error(const xmlpp::ustring &text) {
    std::cout << "on_fatal_error(): " << text << std::endl;
}

std::vector<Page> MySaxParser::GetPages() { return pages; }
