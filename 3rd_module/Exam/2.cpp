#include <iostream>
#include <vector>
#include <cstddef>
#include <cassert>

// Нужно проверить, является ли путь в неориентированном графе гамильтоновым. 
// Граф должен быть реализован в виде класса.

using Vertex = std::size_t;

// weightedVertex.first is vertex, second is weight
struct weightedVertex {
public:
    Vertex vertex;
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
    void addEdge(Vertex from, Vertex to, std::size_t weight = 1);
    std::size_t verticesCount() const;
    std::vector<weightedVertex> getNextVertices(std::size_t vertex) const;
    std::vector<weightedVertex> getPrevVertices(std::size_t vertex) const;

    bool isPathHamiltonian(
        const std::vector<std::size_t> &vertices
    ) const;
};

int main() {
    std::size_t n, m, k;
    std::cin >> n >> m >> k;

    ListGraph graph(n);

    while (m--) {
        Vertex v1, v2;
        std::cin >> v1 >> v2;
        
        graph.addEdge(v1, v2);
        graph.addEdge(v2, v1);
    }

    std::vector<Vertex> path(k);
    for (std::size_t i = 0; i < k; ++i) {
        std::cin >> path[i];
    }
    std::cout << graph.isPathHamiltonian(path) << std::endl;
    
    return 0;
}

bool ListGraph::isPathHamiltonian(
    const std::vector<std::size_t> &vertices
) const {
    // 1. A Hamiltonian path should include every vertex of graph
    if (vertices.size() != verticesCount()) {
        return false;
    }

    // 2. A Hamiltonian path is simple (every vertex is being used at once)
    std::vector<bool> visited(verticesCount(), false);
    for (const auto &v : vertices) {
        if (visited[v] == true) {
            return false;
        }
        visited[v] = true;
    }

    // 3. Every vertex pair should have an edge between
    for (auto i = 0; i < vertices.size() - 1; i++) {
        Vertex current = vertices[i];
        Vertex next = vertices[i + 1];

        bool hasEdge = false;
        for (auto &v : this->adjacencyLists.at(current)) {
            if (v.vertex == next) {
                hasEdge = true;
                break;
            }
        }
        if (hasEdge == false) {
            return false;
        }
    }

    return true;
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