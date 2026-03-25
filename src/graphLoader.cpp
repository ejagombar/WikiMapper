#include "./graph.hpp"
#include "./interface.hpp"
#include <unordered_map>
#include <unordered_set>

class GraphLoader {
  private:
    GS::Graph *m_graph;

    // Fast lookup: Map "PageName" -> Graph Index
    std::unordered_map<std::string, uint32_t> m_nodeMap;

    // Helper to keep track of edges we have already added to avoid
    // calling Graph::AddEdge (which does a linear scan) unnecessarily.
    // Key is combined hash of (u, v) or string pair.
    struct EdgeHash {
        size_t operator()(const std::pair<uint32_t, uint32_t> &k) const { return (size_t)k.first << 32 | k.second; }
    };
    std::unordered_set<std::pair<uint32_t, uint32_t>, EdgeHash> m_existingEdges;

  public:
    GraphLoader(GS::Graph *graph) : m_graph(graph) {}

    void RegisterExistingNode(const std::string &pageName, uint32_t idx) { m_nodeMap[pageName] = idx; }

    // Reset internal maps if the graph is cleared
    void Clear() {
        m_nodeMap.clear();
        m_existingEdges.clear();
    }

    // Returns a list of all PageNames currently in the graph (For Workflow 3)
    std::vector<std::string> GetActivePageNames() const {
        std::vector<std::string> names;
        names.reserve(m_nodeMap.size());
        for (const auto &kv : m_nodeMap) {
            names.push_back(kv.first);
        }
        return names;
    }

    void IngestData(const GraphUpdateData &data) {
        if (!m_graph)
            return;

        // 1. Ensure all Nodes exist
        for (const auto &node : data.nodes) {
            if (m_nodeMap.find(node.pageName) == m_nodeMap.end()) {
                // Node doesn't exist, add to Graph
                uint32_t newIdx = m_graph->AddNode(node.title);
                m_nodeMap[node.pageName] = newIdx;
            }
        }

        // 2. Add Edges
        for (const auto &edge : data.edges) {
            auto itSrc = m_nodeMap.find(edge.sourcePageName);
            auto itTgt = m_nodeMap.find(edge.targetPageName);

            // Sanity check: both nodes must exist now
            if (itSrc != m_nodeMap.end() && itTgt != m_nodeMap.end()) {
                uint32_t u = itSrc->second;
                uint32_t v = itTgt->second;

                if (u == v)
                    continue;
                if (u > v)
                    std::swap(u, v); // Enforce generic order for deduplication

                // Check generic set first to avoid Graph::AddEdge's linear scan
                if (m_existingEdges.find({u, v}) == m_existingEdges.end()) {
                    m_graph->AddEdge(u, v);
                    m_existingEdges.insert({u, v});
                }
            }
        }
    }
};
