package main

import (
	"bytes"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os"
	"strconv"
	"strings"
)

type LinkedPage struct {
	PageName string `json:"pageName"`
	Title    string `json:"title"`
}

type Neo4jClient struct {
	URL      string
	Username string
	Password string
	Client   *http.Client
}

func NewNeo4jClient(url, username, password string) *Neo4jClient {
	return &Neo4jClient{
		URL:      url,
		Username: username,
		Password: password,
		Client:   &http.Client{},
	}
}

func (c *Neo4jClient) ExecuteCypher(cypher string, params map[string]interface{}) (map[string]interface{}, error) {
	query := map[string]interface{}{
		"statements": []map[string]interface{}{
			{
				"statement":  cypher,
				"parameters": params,
			},
		},
	}
	payload, err := json.Marshal(query)
	if err != nil {
		return nil, err
	}

	req, err := http.NewRequest("POST", c.URL+"/db/neo4j/tx/commit", bytes.NewBuffer(payload))
	if err != nil {
		return nil, err
	}

	req.Header.Set("Content-Type", "application/json")
	auth := base64.StdEncoding.EncodeToString([]byte(c.Username + ":" + c.Password))
	req.Header.Set("Authorization", "Basic "+auth)

	resp, err := c.Client.Do(req)
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("HTTP error %d", resp.StatusCode)
	}

	var data map[string]interface{}
	if err := json.NewDecoder(resp.Body).Decode(&data); err != nil {
		return nil, err
	}

	if errs, ok := data["errors"].([]interface{}); ok && len(errs) > 0 {
		return nil, fmt.Errorf("Cypher error: %v", errs)
	}

	return data, nil
}

// ParsePages parses Cypher result into LinkedPage slice.
func ParsePages(data map[string]interface{}) ([]LinkedPage, error) {
	var pages []LinkedPage
	results := data["results"].([]interface{})
	if len(results) == 0 {
		return pages, nil
	}

	first := results[0].(map[string]interface{})
	rows := first["data"].([]interface{})

	for _, r := range rows {
		row := r.(map[string]interface{})
		node := row["row"].([]interface{})[0].(map[string]interface{})
		pages = append(pages, LinkedPage{
			PageName: node["pageName"].(string),
			Title:    node["title"].(string),
		})
	}

	return pages, nil
}

// Handler functions
func linkedPagesHandler(client *Neo4jClient) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		// Extract path parameter manually: /linked-pages/{name}
		parts := strings.Split(r.URL.Path, "/")
		if len(parts) != 3 || parts[1] != "linked-pages" {
			http.Error(w, "Invalid path", http.StatusBadRequest)
			return
		}
		name := parts[2]

		cypher := `MATCH (:PAGE {pageName: $name})-[]->(related:PAGE) RETURN related`
		params := map[string]interface{}{"name": name}

		data, err := client.ExecuteCypher(cypher, params)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		pages, err := ParsePages(data)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		json.NewEncoder(w).Encode(pages)
	}
}

func linkingPagesHandler(client *Neo4jClient) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		// Extract path parameter manually: /linking-pages/{name}
		parts := strings.Split(r.URL.Path, "/")
		if len(parts) != 3 || parts[1] != "linking-pages" {
			http.Error(w, "Invalid path", http.StatusBadRequest)
			return
		}
		name := parts[2]

		cypher := `MATCH (related:PAGE)-[r]->(:PAGE {pageName: $name}) RETURN related`
		params := map[string]interface{}{"name": name}

		data, err := client.ExecuteCypher(cypher, params)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		pages, err := ParsePages(data)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		json.NewEncoder(w).Encode(pages)
	}
}

func shortestPathHandler(client *Neo4jClient) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		start := r.URL.Query().Get("start")
		end := r.URL.Query().Get("end")
		if start == "" || end == "" {
			http.Error(w, "Missing start or end query parameter", http.StatusBadRequest)
			return
		}

		cypher := `MATCH path = shortestPath((start:PAGE {pageName: $startName})-[*]-(end:PAGE {pageName: $endName})) RETURN nodes(path) AS nodes`
		params := map[string]interface{}{"startName": start, "endName": end}

		data, err := client.ExecuteCypher(cypher, params)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		var pages []LinkedPage
		nodes := data["results"].([]interface{})[0].(map[string]interface{})["data"].([]interface{})[0].(map[string]interface{})["row"].([]interface{})[0].([]interface{})
		for _, n := range nodes {
			node := n.(map[string]interface{})
			pages = append(pages, LinkedPage{
				PageName: node["pageName"].(string),
				Title:    node["title"].(string),
			})
		}

		json.NewEncoder(w).Encode(pages)
	}
}

func randomPagesHandler(client *Neo4jClient) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		countStr := r.URL.Query().Get("count")
		count, err := strconv.Atoi(countStr)
		if err != nil || count <= 0 {
			count = 5
		}

		cypher := `MATCH (p:PAGE) WITH p, rand() AS r ORDER BY r RETURN p LIMIT $limit`
		params := map[string]interface{}{"limit": count}

		data, err := client.ExecuteCypher(cypher, params)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		pages, err := ParsePages(data)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		json.NewEncoder(w).Encode(pages)
	}
}

func main() {
	neo4jURL := os.Getenv("NEO4J_URL")
	username := os.Getenv("NEO4J_USER")
	password := os.Getenv("NEO4J_PASS")

	if neo4jURL == "" || username == "" || password == "" {
		log.Fatal("Please set NEO4J_URL, NEO4J_USER, and NEO4J_PASS environment variables")
	}

	client := NewNeo4jClient(neo4jURL, username, password)

	http.HandleFunc("/linked-pages/", linkedPagesHandler(client))
	http.HandleFunc("/linking-pages/", linkingPagesHandler(client))
	http.HandleFunc("/shortest-path", shortestPathHandler(client))
	http.HandleFunc("/random-pages", randomPagesHandler(client))

	port := ":8080"
	fmt.Printf("Server listening on %s...\n", port)
	log.Fatal(http.ListenAndServe(port, nil))
}
