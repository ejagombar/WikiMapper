#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "saxparser.h"
#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <json/json.h>
#include <string>

void addPageToNeo4j(const std::string &neo4jUrl, const std::string &username, const std::string &password,
                    const Page &page) {
    // Initialize cURL
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize cURL" << std::endl;
        return;
    }

    // Create the JSON payload
    Json::Value jsonPayload;
    jsonPayload["statements"][0]["statement"] = "CREATE (p:Page {title: $title, redirect: $redirect}) "
                                                "WITH p "
                                                "UNWIND $links AS link "
                                                "MATCH (l:Page {title: link}) "
                                                "CREATE (p)-[:LINKS_TO]->(l)";
    jsonPayload["statements"][0]["parameters"]["title"] = page.title;
    jsonPayload["statements"][0]["parameters"]["redirect"] = page.redirect;
    for (const auto &link : page.links) {
        jsonPayload["statements"][0]["parameters"]["links"].append(link);
    }

    Json::StreamWriterBuilder writer;
    std::string payload = Json::writeString(writer, jsonPayload);
    // std::cout << payload << std::endl;

    // Set the cURL options
    curl_easy_setopt(curl, CURLOPT_URL, (neo4jUrl + "/db/neo4j/tx/commit").c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_USERPWD, (username + ":" + password).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Content-Type: application/json"));

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "Page added successfully to Neo4j." << std::endl;
    }

    // Clean up
    curl_easy_cleanup(curl);
}

void sendPageNeo4j(Page page) {
    const std::string url = "http://localhost:7474";
    const std::string username = "neo4j";
    const std::string password = "test1234";
    addPageToNeo4j(url, username, password, page);
}

int main(int argc, char *argv[]) {

    int return_code = EXIT_SUCCESS;

    if (argc <= 1) {
        std::cerr << "No input file provided" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filepath = argv[1];

    // Parse document in one go
    try {
        MySaxParser parser(&sendPageNeo4j);
        parser.set_substitute_entities(true);
        parser.parse_file(filepath);

    } catch (const xmlpp::exception &ex) {
        std::cerr << "libxml++ exception: " << ex.what() << std::endl;
        return_code = EXIT_FAILURE;
    }

    return return_code;
}
