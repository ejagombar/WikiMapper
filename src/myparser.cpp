#include "myparser.h"

#include <iostream>

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

    // Print attributes:
    for (const auto &attr_pair : attributes) {
        std::cout << "  Attribute name=" << attr_pair.name << std::endl;
        std::cout << "    , value= " << attr_pair.value << std::endl;
    }
}

void MySaxParser::on_end_element(const xmlpp::ustring & /* name */) {
    std::cout << "on_end_element()" << std::endl;
}

void MySaxParser::on_characters(const xmlpp::ustring &text) {
    std::cout << "on_characters(): " << text << std::endl;
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
