#ifndef __LIBXMLPP_EXAMPLES_MYPARSER_H
#define __LIBXMLPP_EXAMPLES_MYPARSER_H

#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <libxml++/libxml++.h>
#include <ostream>
#include <re2/re2.h>
#include <vector>

struct Page {
    std::string title;
    bool redirect = false;
    std::vector<std::string> links;
};

enum ElementType { TITLE, CONTENT, OTHER };

class MySaxParser : public xmlpp::SaxParser {
  public:
    MySaxParser();
    ~MySaxParser() override;
    std::vector<Page> GetPages();

  private:
    int depth = 0;
    std::atomic<int> processedPageCount = 0;
    std::atomic<bool> stopOutputThread = false;
    Page page;
    std::vector<Page> pages;
    ElementType nextElement;
    std::string content;
    std::ofstream CSVFile;

    void ExtractAllLinks();
    void OutputPageCount();

  protected:
    // overrides:
    void on_start_document() override;
    void on_end_document() override;
    void on_start_element(const xmlpp::ustring &name,
                          const AttributeList &properties) override;
    void on_end_element(const xmlpp::ustring &name) override;
    void on_characters(const xmlpp::ustring &characters) override;
    void on_comment(const xmlpp::ustring &text) override;
    void on_warning(const xmlpp::ustring &text) override;
    void on_error(const xmlpp::ustring &text) override;
    void on_fatal_error(const xmlpp::ustring &text) override;
};

#endif //__LIBXMLPP_EXAMPLES_MYPARSER_H
