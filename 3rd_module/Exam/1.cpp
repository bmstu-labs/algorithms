#include <iostream>
#include <vector>
#include <cstddef>
#include <cassert>

// weightedVertex.first is vertex, second is weight
struct weightedVertex {
public:
    std::size_t vertex;
    std::size_t weight;
public:
    weightedVertex(
        std::size_t v, 
        std::size_t w = 1
    ) : vertex(v), weight(w) {}
};

// Хранит граф в виде массива списков смежности
class ListGraph {
private:
    std::vector<std::vector<weightedVertex>> adjacencyLists; 
public:
    ListGraph(std::size_t size);
    ~ListGraph() = default;
public:
    void addEdge(std::size_t from, std::size_t to, std::size_t weight = 1);
    std::size_t verticesCount() const;
    std::vector<weightedVertex> getNextVertices(std::size_t vertex) const;
    std::vector<weightedVertex> getPrevVertices(std::size_t vertex) const;
};

void dfs(
    const ListGraph &graph,
    std::size_t startVertex,
    std::vector<bool> &visited
) {
    visited[startVertex] = true;
    for (const auto &neibor : graph.getNextVertices(startVertex)) {
        if (visited[neibor.vertex] == false) {
            dfs(graph, neibor.vertex, visited);
        }
    }
}

std::size_t countComponents(const ListGraph &graph) {
    const std::size_t n = graph.verticesCount();
    std::vector<bool> visited(n, false);

    std::size_t components = 0;
    for (std::size_t vertex = 0; vertex < n; vertex++) {
        if (visited[vertex] == false) {
            dfs(graph, vertex, visited);
            components++;
        }
    }

    return components;
}

int main() {
    std::size_t n, m;
    std::cin >> n >> m;

    ListGraph graph(n);

    while (m--) {
        std::size_t v1, v2;
        std::cin >> v1 >> v2;

        graph.addEdge(v1, v2);
        graph.addEdge(v2, v1);
    }

    std::cout << countComponents(graph) << std::endl;
    return 0;
}

ListGraph::ListGraph(
    std::size_t size
) : adjacencyLists(size) {}


void ListGraph::addEdge(
    std::size_t from,
    std::size_t to,
    std::size_t weight
) {
    assert(0 <= from && from < adjacencyLists.size());
    assert(0 <= to && to < adjacencyLists.size());

    weightedVertex v(to, weight);
    adjacencyLists[from].push_back(v);
}

std::size_t ListGraph::verticesCount() const {
    return adjacencyLists.size();
}

std::vector<weightedVertex> ListGraph::getNextVertices(
    std::size_t vertex
) const {
    assert(0 <= vertex && vertex < adjacencyLists.size());
    return adjacencyLists[vertex];
}

std::vector<weightedVertex> ListGraph::getPrevVertices(
    std::size_t vertex
) const {
    assert(0 <= vertex && vertex < adjacencyLists.size());

    std::vector<weightedVertex> prevVertices;
    for (auto from = 0; from < adjacencyLists.size(); from++) {
        for (auto to : adjacencyLists[from]) {
            if (to.vertex == vertex) {
                weightedVertex v(from, to.weight);
                prevVertices.push_back(v);
            }
        }
    }

    return std::move(prevVertices);
}