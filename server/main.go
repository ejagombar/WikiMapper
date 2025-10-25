package main

import (
	"bytes"
	"context"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os"
	"os/signal"
	"strconv"
	"strings"
	"syscall"
	"time"
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

func (c *Neo4jClient) ExecuteCypher(cypher string, params map[string]any) (map[string]any, error) {
	query := map[string]any{
		"statements": []map[string]any{
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

	endpoint := fmt.Sprintf("%s/db/neo4j/tx/commit", c.URL)
	req, err := http.NewRequest("POST", endpoint, bytes.NewBuffer(payload))
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

	var data map[string]any
	if err := json.NewDecoder(resp.Body).Decode(&data); err != nil {
		return nil, err
	}

	if errs, ok := data["errors"].([]any); ok && len(errs) > 0 {
		return nil, fmt.Errorf("Cypher error: %v", errs)
	}

	return data, nil
}

// ParsePages parses Cypher result into LinkedPage slice.
func ParsePages(data map[string]any) ([]LinkedPage, error) {
	var pages []LinkedPage
	results := data["results"].([]any)
	if len(results) == 0 {
		return pages, nil
	}

	first := results[0].(map[string]any)
	rows := first["data"].([]any)

	for _, r := range rows {
		row := r.(map[string]any)
		node := row["row"].([]any)[0].(map[string]any)
		pages = append(pages, LinkedPage{
			PageName: node["pageName"].(string),
			Title:    node["title"].(string),
		})
	}

	return pages, nil
}

func linkedPagesHandler(client *Neo4jClient) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		parts := strings.Split(r.URL.Path, "/")
		if len(parts) != 3 || parts[1] != "linked-pages" {
			http.Error(w, "Invalid path", http.StatusBadRequest)
			return
		}
		name := parts[2]

		cypher := `MATCH (:PAGE {pageName: $name})-[]->(related:PAGE) RETURN related`
		params := map[string]any{"name": name}

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
		parts := strings.Split(r.URL.Path, "/")
		if len(parts) != 3 || parts[1] != "linking-pages" {
			http.Error(w, "Invalid path", http.StatusBadRequest)
			return
		}
		name := parts[2]

		cypher := `MATCH (related:PAGE)-[r]->(:PAGE {pageName: $name}) RETURN related`
		params := map[string]any{"name": name}

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
		params := map[string]any{"startName": start, "endName": end}

		data, err := client.ExecuteCypher(cypher, params)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		var pages []LinkedPage
		nodes := data["results"].([]any)[0].(map[string]any)["data"].([]any)[0].(map[string]any)["row"].([]any)[0].([]any)
		for _, n := range nodes {
			node := n.(map[string]any)
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
		params := map[string]any{"limit": count}

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

func connectedHandler(client *Neo4jClient) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		cypher := `RETURN 1 AS test`

		data, err := client.ExecuteCypher(cypher, nil)
		if err != nil {
			http.Error(w, fmt.Sprintf("Database not accessible: %v", err), http.StatusServiceUnavailable)
			return
		}

		results, ok := data["results"].([]any)
		if !ok || len(results) == 0 {
			http.Error(w, "Unexpected response from database", http.StatusInternalServerError)
			return
		}

		first := results[0].(map[string]any)
		rows := first["data"].([]any)
		if len(rows) == 0 {
			http.Error(w, "No data returned from test query", http.StatusInternalServerError)
			return
		}

		row := rows[0].(map[string]any)
		value := row["row"].([]any)[0]

		resp := map[string]any{
			"connected": true,
			"testValue": value,
		}

		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(resp)
	}
}

func main() {
	neo4jURL := os.Getenv("NEO4J_URL")
	username := os.Getenv("NEO4J_USER")
	password := os.Getenv("NEO4J_PASS")
	port := os.Getenv("API_PORT")

	if neo4jURL == "" || username == "" || password == "" {
		log.Fatal("Please set NEO4J_URL, NEO4J_USER, and NEO4J_PASS environment variables")
	}

	client := NewNeo4jClient(neo4jURL, username, password)

	mux := http.NewServeMux()
	mux.HandleFunc("/linked-pages/", linkedPagesHandler(client))
	mux.HandleFunc("/linking-pages/", linkingPagesHandler(client))
	mux.HandleFunc("/shortest-path", shortestPathHandler(client))
	mux.HandleFunc("/random-pages", randomPagesHandler(client))
	mux.HandleFunc("/connected", connectedHandler(client))

	server := &http.Server{
		Addr:    ":" + port,
		Handler: mux,
	}

	// Graceful shutdown
	idleConnsClosed := make(chan struct{})

	go func() {
		sigint := make(chan os.Signal, 1)
		signal.Notify(sigint, os.Interrupt, syscall.SIGTERM)
		<-sigint

		ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
		defer cancel()

		if err := server.Shutdown(ctx); err != nil {
			log.Printf("HTTP server Shutdown: %v", err)
		}
		close(idleConnsClosed)
	}()

	fmt.Println("Server is running on " + port + ":")
	if err := server.ListenAndServe(); err != http.ErrServerClosed {
		log.Fatalf("HTTP server ListenAndServe: %v", err)
	}

	<-idleConnsClosed
	fmt.Println("Server stopped")
}
