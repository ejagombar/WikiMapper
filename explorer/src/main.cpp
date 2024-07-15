#include <iostream>
#include <neo4j-client.h>

int main() {
    // Initialize the Neo4j client
    if (neo4j_client_init() != 0) {
        std::cerr << "Failed to initialize Neo4j client" << std::endl;
        return 1;
    }

    // Set up the connection URL and credentials
    const char *url = "bolt://username:password@localhost:7687";
    neo4j_connection_t *connection = neo4j_connect(url, NULL, NEO4J_INSECURE);
    if (connection == NULL) {
        std::cerr << "Failed to connect to Neo4j database: " << neo4j_strerror(errno) << std::endl;
        neo4j_client_cleanup();
        return 1;
    }

    // Execute a simple query
    const char *query = "MATCH (n) RETURN n LIMIT 10";
    neo4j_result_stream_t *results = neo4j_run(connection, query, neo4j_null);
    if (results == NULL) {
        std::cerr << "Failed to execute query: " << neo4j_strerror(errno) << std::endl;
        neo4j_close(connection);
        neo4j_client_cleanup();
        return 1;
    }

    // Fetch and print the results
    neo4j_result_t *result;
    while ((result = neo4j_fetch_next(results)) != NULL) {
        neo4j_value_t value = neo4j_result_field(result, 0);
        char buf[256];
        neo4j_tostring(value, buf, sizeof(buf));
        std::cout << "Node: " << buf << std::endl;
    }

    // Clean up
    neo4j_close(connection);
    neo4j_client_cleanup();

    return 0;
}
