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

type GraphEdge struct {
	Source string `json:"source"`
	Target string `json:"target"`
}

type GraphResult struct {
	Nodes []LinkedPage `json:"nodes"`
	Edges []GraphEdge  `json:"edges"`
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

// ParseGraphResult parses a Cypher result where each row is [sourceNode, targetNode].
func ParseGraphResult(data map[string]any) (GraphResult, error) {
	var result GraphResult
	results, ok := data["results"].([]any)
	if !ok || len(results) == 0 {
		return result, nil
	}
	first := results[0].(map[string]any)
	rows, _ := first["data"].([]any)
	for _, r := range rows {
		row, ok := r.(map[string]any)["row"].([]any)
		if !ok || len(row) < 2 {
			continue
		}
		src, ok1 := row[0].(map[string]any)
		tgt, ok2 := row[1].(map[string]any)
		if !ok1 || !ok2 {
			continue
		}
		srcName, _ := src["pageName"].(string)
		srcTitle, _ := src["title"].(string)
		tgtName, _ := tgt["pageName"].(string)
		tgtTitle, _ := tgt["title"].(string)
		result.Nodes = append(result.Nodes, LinkedPage{PageName: srcName, Title: srcTitle})
		result.Nodes = append(result.Nodes, LinkedPage{PageName: tgtName, Title: tgtTitle})
		result.Edges = append(result.Edges, GraphEdge{Source: srcName, Target: tgtName})
	}
	return result, nil
}

func linkedPagesHandler(client *Neo4jClient) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		parts := strings.Split(r.URL.Path, "/")
		if len(parts) != 3 || parts[1] != "linked-pages" {
			http.Error(w, "Invalid path", http.StatusBadRequest)
			return
		}
		name := parts[2]

		cypher := `MATCH (p:PAGE)-[]->(related:PAGE) WHERE p.pageName = $name OR p.title = $name RETURN related`
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

func randomConnectedPageHandler(client *Neo4jClient) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		if r.Method != http.MethodPost {
			http.Error(w, "POST required", http.StatusMethodNotAllowed)
			return
		}

		var body struct {
			Names []string `json:"names"`
		}
		if err := json.NewDecoder(r.Body).Decode(&body); err != nil || len(body.Names) == 0 {
			http.Error(w, "Invalid request body", http.StatusBadRequest)
			return
		}

		cypher := `
			UNWIND $names AS name
			MATCH (a:PAGE {pageName: name})-[]-(neighbor:PAGE)
			WHERE NOT neighbor.pageName IN $names
			WITH neighbor, rand() AS r
			ORDER BY r
			RETURN neighbor
			LIMIT 1`
		params := map[string]any{"names": body.Names}

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

		w.Header().Set("Content-Type", "application/json")
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

func searchPagesHandler(client *Neo4jClient) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		query := r.URL.Query().Get("query")
		if query == "" {
			http.Error(w, "Missing query parameter", http.StatusBadRequest)
			return
		}

		cypher := `
            MATCH (p:PAGE)
            WHERE p.pageName STARTS WITH toLower($query)
            RETURN p.pageName AS pageName, p.title AS title
            LIMIT 25
        `
		params := map[string]any{"query": query}

		data, err := client.ExecuteCypher(cypher, params)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		// Parse manually because the return is scalar (no nested node map)
		var pages []LinkedPage
		results := data["results"].([]any)
		if len(results) > 0 {
			first := results[0].(map[string]any)
			rows := first["data"].([]any)
			for _, r := range rows {
				row := r.(map[string]any)
				values := row["row"].([]any)
				if len(values) >= 2 {
					pages = append(pages, LinkedPage{
						PageName: values[0].(string),
						Title:    values[1].(string),
					})
				}
			}
		}

		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(pages)
	}
}

func localSubgraphHandler(client *Neo4jClient) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		if r.Method != http.MethodPost {
			http.Error(w, "POST required", http.StatusMethodNotAllowed)
			return
		}

		var body struct {
			Name  string `json:"name"`
			Limit int    `json:"limit"`
		}
		body.Limit = 500
		if err := json.NewDecoder(r.Body).Decode(&body); err != nil || body.Name == "" {
			http.Error(w, "Invalid request body", http.StatusBadRequest)
			return
		}
		if body.Limit <= 0 {
			body.Limit = 500
		}

		cypher := `MATCH (n:PAGE)-[r]-(m:PAGE)
WHERE n.pageName = toLower($name)
RETURN {pageName: n.pageName, title: n.title} AS source,
       {pageName: m.pageName, title: m.title} AS target
LIMIT $limit`
		params := map[string]any{"name": body.Name, "limit": body.Limit}

		data, err := client.ExecuteCypher(cypher, params)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		result, err := ParseGraphResult(data)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(result)
	}
}

func interconnectionsHandler(client *Neo4jClient) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		if r.Method != http.MethodPost {
			http.Error(w, "POST required", http.StatusMethodNotAllowed)
			return
		}

		var body struct {
			Names []string `json:"names"`
			Limit int      `json:"limit"`
		}
		body.Limit = 500
		if err := json.NewDecoder(r.Body).Decode(&body); err != nil || len(body.Names) == 0 {
			http.Error(w, "Invalid request body", http.StatusBadRequest)
			return
		}
		if body.Limit <= 0 {
			body.Limit = 500
		}

		cypher := `UNWIND $names AS name
MATCH (a:PAGE {pageName: name})-[]->(b:PAGE)
WHERE b.pageName IN $names
RETURN {pageName: a.pageName, title: a.title} AS source,
       {pageName: b.pageName, title: b.title} AS target
LIMIT $limit`
		params := map[string]any{"names": body.Names, "limit": body.Limit}

		data, err := client.ExecuteCypher(cypher, params)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		result, err := ParseGraphResult(data)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}

		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(result)
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
	mux.HandleFunc("/random-connected-page", randomConnectedPageHandler(client))
	mux.HandleFunc("/connected", connectedHandler(client))
	mux.HandleFunc("/search-pages", searchPagesHandler(client))
	mux.HandleFunc("/local-subgraph", localSubgraphHandler(client))
	mux.HandleFunc("/interconnections", interconnectionsHandler(client))

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
