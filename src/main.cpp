#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <libxml++/libxml++.h>

class MySaxParser : public xmlpp::SaxParser {
  public:
    MySaxParser();
    virtual ~MySaxParser();

  protected:
    // overrides:
#ifdef LIBXMLCPP_EXCEPTIONS_ENABLED
    virtual void on_start_document();
    virtual void on_end_document();
    virtual void on_start_element(const xmlpp::string &name,
                                  const AttributeList &properties);
    virtual void on_end_element(const xmlpp::string &name);
    virtual void on_characters(const xmlpp::string &characters);
    virtual void on_comment(const xmlpp::string &text);
    virtual void on_warning(const xmlpp::string &text);
    virtual void on_error(const xmlpp::string &text);
    virtual void on_fatal_error(const xmlpp::string &text);
#endif
};

#include <iostream>

MySaxParser::MySaxParser() : xmlpp::SaxParser() {}

MySaxParser::~MySaxParser() {}

#ifdef LIBXMLCPP_EXCEPTIONS_ENABLED
void MySaxParser::on_start_document() {
    std::cout << "on_start_document()" << std::endl;
}

void MySaxParser::on_end_document() {
    std::cout << "on_end_document()" << std::endl;
}

void MySaxParser::on_start_element(const xmlpp::string &name,
                                   const AttributeList &attributes) {
    std::cout << "node name=" << name << std::endl;

    // Print attributes:
    for (xmlpp::SaxParser::AttributeList::const_iterator iter =
             attributes.begin();
         iter != attributes.end(); ++iter) {
        std::cout << "  Attribute name=" << iter->name << std::endl;

        std::cout << "    , value= " << iter->value << std::endl;
    }
}

void MySaxParser::on_end_element(const xmlpp::string & /* name */) {
    std::cout << "on_end_element()" << std::endl;
}

void MySaxParser::on_characters(const xmlpp::string &text) {
    std::cout << "on_characters(): " << text << std::endl;
}

void MySaxParser::on_comment(const xmlpp::string &text) {
    std::cout << "on_comment(): " << text << std::endl;
}

void MySaxParser::on_warning(const xmlpp::string &text) {
    std::cout << "on_warning(): " << text << std::endl;
}

void MySaxParser::on_error(const xmlpp::string &text) {
    std::cout << "on_error(): " << text << std::endl;
}

void MySaxParser::on_fatal_error(const xmlpp::string &text) {
    std::cout << "on_fatal_error(): " << text << std::endl;
}
#endif

// int main() {
//     std::cout << "Hello World" << std::endl;
//     std::cout << "Hello World 2" << std::endl;
//     fmt::print("Hello World 3\n");
// }

int main(int argc, char *argv[]) {
    // Set the global C and C++ locale to the user-configured locale,
    // so we can use std::cout with UTF-8, via xmlpp::string, without
    // exceptions.
    std::locale::global(std::locale(""));

    std::string filepath;
    if (argc > 1)
        filepath =
            argv[1]; // Allow the user to specify a different XML file to parse.
    else
        filepath = "example.xml";

// Parse the entire document in one go:
#ifdef LIBXMLCPP_EXCEPTIONS_ENABLED
    try {
#endif // LIBXMLCPP_EXCEPTIONS_ENABLED
        MySaxParser parser;
        parser.set_substitute_entities(true); //
        parser.parse_file(filepath);
#ifdef LIBXMLCPP_EXCEPTIONS_ENABLED
    } catch (const xmlpp::exception &ex) {
        std::cout << "libxml++ exception: " << ex.what() << std::endl;
    }
#endif // LIBXMLCPP_EXCEPTIONS_ENABLED

    // Demonstrate incremental parsing, sometimes useful for network
    // connections:
    {
        // std::cout << "Incremental SAX Parser:" << std:endl;

        std::ifstream is(filepath.c_str());
        /* char buffer[64];
        const size_t buffer_size = sizeof(buffer) / sizeof(char); */

        // Parse the file:
        MySaxParser parser;
        parser.parse_file(filepath);

        // Or parse chunks (though this seems to have problems):
        /*
            do
            {
              memset(buffer, 0, buffer_size);
              is.read(buffer, buffer_size-1);
              if(is && is.gcount())
              {
                xmlpp::string input(buffer, is.gcount());
                parser.parse_chunk(input);
              }
            }
            while(is);

            parser.finish_chunk_parsing();
        */
    }

    return 0;
}
