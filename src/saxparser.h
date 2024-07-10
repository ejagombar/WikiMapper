#ifndef __LIBXMLPP_EXAMPLES_MYPARSER_H
#define __LIBXMLPP_EXAMPLES_MYPARSER_H

#include <bits/stdc++.h>
#include <ctime>
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
    void Clear();

  private:
    Page page;
    std::string content;
    std::vector<Page> pages;

    int depth = 0;
    ElementType nextElement;
    int count = 0;

    void ExtractAllLinks();
    void OutputPageCount();
    void FormatLink(std::string &str);

  protected:
    // overrides:
    void on_start_document() override;
    void on_end_document() override;
    void on_start_element(const xmlpp::ustring &name, const AttributeList &properties) override;
    void on_end_element(const xmlpp::ustring &name) override;
    void on_characters(const xmlpp::ustring &characters) override;
    void on_comment(const xmlpp::ustring &text) override;
    void on_warning(const xmlpp::ustring &text) override;
    void on_error(const xmlpp::ustring &text) override;
    void on_fatal_error(const xmlpp::ustring &text) override;
};

#endif //__LIBXMLPP_EXAMPLES_MYPARSER_H
