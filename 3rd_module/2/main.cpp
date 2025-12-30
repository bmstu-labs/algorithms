#include <vector>
#include <cstddef>
#include <vector>
#include <queue>
#include <utility>
#include <iostream>
#include <cassert>
#include <limits>

// Дан невзвешенный неориентированный граф. 
// В графе может быть несколько кратчайших путей между какими-то вершинами.
// Найдите количество различных кратчайших путей между заданными вершинами. 

namespace {    
    struct IGraph {
    public:
        virtual ~IGraph() {}
    public: 
        // Добавление связи между вершинами
        virtual void addEdge(
            std::size_t from,
            std::size_t to
        ) = 0;

        // Подсчет количества вершин
        virtual std::size_t verticesCount() const  = 0;
        
        // Получение вершин, на которые указывает данная
        virtual std::vector<std::size_t> getNextVertices(
            std::size_t vertex
        ) const = 0;
        
        // Получение вершин, которые указывают на данную
        virtual std::vector<std::size_t> getPrevVertices(
            std::size_t vertex
        ) const = 0;
    };
}

// Хранит граф в виде массива списков смежности
class ListGraph : public IGraph {
private:
    std::vector<std::vector<std::size_t>> adjacencyLists; 
public:
    ListGraph(std::size_t size);
    ListGraph(const IGraph &);
    ~ListGraph() = default;
public:
    void addEdge(std::size_t from, std::size_t to) override;
    std::size_t verticesCount() const  override;
    std::vector<std::size_t> getNextVertices(std::size_t vertex) const override;
    std::vector<std::size_t> getPrevVertices(std::size_t vertex) const override;
};

// pair.first is path length, pair.second is count of the shortest paths
std::pair<std::size_t, std::size_t> findShortestPaths(
    const IGraph &graph,
    std::size_t startVertex,
    std::size_t endVertex
) {
    if (graph.verticesCount() <= endVertex) {
        return {0, 0};
    }

    const std::size_t n = graph.verticesCount();
    std::vector<std::size_t> r(n, std::numeric_limits<std::size_t>::max());
    std::vector<std::size_t> k(n, 0);

    std::queue<std::size_t> q;

    r[startVertex] = 0;
    k[startVertex] = 1;
    q.push(startVertex);

    while (q.empty() == false) {
        auto u = q.front();
        q.pop();

        for (auto v : graph.getNextVertices(u)) {
            if (r[v] > r[u] + 1) {
                r[v] = r[u] + 1;
                k[v] = k[u];
                q.push(v);
            }
            else if (r[v] == r[u] + 1) {
                k[v] += k[u];
            }
        }
    }

    if (r[endVertex] == std::numeric_limits<std::size_t>::max()) {
        return {0, 0};
    }

    return {r[endVertex], k[endVertex]};
}

int main() {

    std::size_t v, n;
    std::cin >> v >> n;

    ListGraph graph(v);

    while (n--) {
        std::size_t from, to;
        std::cin >> from >> to;
        graph.addEdge(from, to);
        graph.addEdge(to, from);
    }

    std::size_t from, to;
    std::cin >> from >> to;
    std::cout << findShortestPaths(graph, from, to).second << std::endl;

    return 0;
}

ListGraph::ListGraph(
    std::size_t size
) : adjacencyLists(size) {}

ListGraph::ListGraph(const IGraph &other) {
    adjacencyLists.resize(other.verticesCount());
    for (auto i = 0; i < adjacencyLists.size(); i++) {
        adjacencyLists[i] = other.getNextVertices(i);
    }
}

void ListGraph::addEdge(
    std::size_t from,
    std::size_t to
) {
    assert(0 <= from && from < adjacencyLists.size());
    assert(0 <= to && to < adjacencyLists.size());

    adjacencyLists[from].push_back(to);
}

std::size_t ListGraph::verticesCount() const {
    return adjacencyLists.size();
}

std::vector<std::size_t> ListGraph::getNextVertices(
    std::size_t vertex
) const {
    assert(0 <= vertex && vertex < adjacencyLists.size());
    return adjacencyLists[vertex];
}

std::vector<std::size_t> ListGraph::getPrevVertices(
    std::size_t vertex
) const {
    assert(0 <= vertex && vertex < adjacencyLists.size());

    std::vector<std::size_t> prevVertices;
    for (auto from = 0; from < adjacencyLists.size(); from++) {
        for (auto to : adjacencyLists[from]) {
            if (to == vertex) {
                prevVertices.push_back(from);
            }
        }
    }

    return std::move(prevVertices);
}