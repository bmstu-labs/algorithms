#include <vector>
#include <cstddef>
#include <vector>
#include <queue>
#include <utility>
#include <iostream>
#include <cassert>
#include <limits>
#include <algorithm>

// Требуется отыскать самый короткий маршрут между городами. 
// Из города может выходить дорога, которая возвращается в этот же город. 

// weightedVertex.first is vertex, second is weight
using weightedVertex = std::pair<std::size_t, std::size_t>;

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

bool relax(
    std::size_t u, 
    std::size_t v, 
    std::size_t weight,
    std::vector<std::size_t> &dist,
    std::vector<std::size_t> &parent
) {
    if (dist[u] + weight < dist[v]) {
        dist[v] = dist[u] + weight;
        parent[v] = u;
        return true;
    }
    return false;
}

// pair.first is path length, pair.second is count of the shortest paths
std::pair<std::size_t, std::vector<std::size_t>> 
findShortestPath(
    const ListGraph &graph,
    std::size_t startVertex,
    std::size_t endVertex
) {
    std::vector<std::size_t> shortestPath;
    const std::size_t n = graph.verticesCount();

    if (startVertex >= n || endVertex >= n) {
        return {0, shortestPath};
    }

    const std::size_t INF = std::numeric_limits<std::size_t>::max();
    std::vector<std::size_t> r(n, INF);
    std::vector<std::size_t> p(n, INF);

    r[startVertex] = 0;

    using QueueNode = std::pair<std::size_t, std::size_t>;
    std::priority_queue<
        QueueNode,
        std::vector<QueueNode>, 
        std::greater<QueueNode>
    > q;
    q.push({0, startVertex});

    while (q.empty() == false) {
        auto [distance, u] = q.top();
        q.pop();

        if (distance > r[u]) {
            continue;
        }

        for (const auto &[v, weight] : graph.getNextVertices(u)) {
            if (relax(u, v, weight, r, p)) {
                q.push({r[v], v});
            }
        }
    }

    if (r[endVertex] == INF) {
        return {0, shortestPath};
    }

    for (std::size_t v = endVertex; v != INF; v = p[v]) {
        shortestPath.push_back(v);
        if (v == startVertex) {
            break;
        }
    }

    std::reverse(shortestPath.begin(), shortestPath.end());

    return {r[endVertex], shortestPath};
}

int main() {
    std::size_t v, n;
    std::cin >> v >> n;

    ListGraph graph(v);

    while (n--) {
        std::size_t from, to, weight;
        std::cin >> from >> to >> weight;
        graph.addEdge(from, to, weight);
        graph.addEdge(to, from, weight);
    }

    std::size_t from, to;
    std::cin >> from >> to;

    std::cout << findShortestPath(graph, from, to).first << std::endl;

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

    adjacencyLists[from].push_back({to, weight});
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
            if (to.first == vertex) {
                prevVertices.push_back({from, to.second});
            }
        }
    }

    return std::move(prevVertices);
}