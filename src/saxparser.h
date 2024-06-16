#ifndef __LIBXMLPP_EXAMPLES_MYPARSER_H
#define __LIBXMLPP_EXAMPLES_MYPARSER_H

#include <bits/stdc++.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <libxml++/libxml++.h>
#include <re2/re2.h>
#include <vector>

#define cursup "\033[A"

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
    std::atomic<std::chrono::time_point<std::chrono::system_clock>> startTime;
    Page page;
    std::vector<Page> pages;
    ElementType nextElement;
    std::string content;
    std::ofstream CSVFileLinks;
    std::ofstream CSVFileNodes;

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
