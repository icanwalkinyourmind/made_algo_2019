/*
 Дан неориентированный связный граф. Требуется найти вес минимального остовного дерева в этом графе.
 Вариант 1. С помощью алгоритма Прима.
*/

#include <iostream>
#include <vector>
#include <queue>
#include <cassert>
#include <functional>

// для каждого ребра храним вершину в которое оно ведёт и его вес
struct Edge {
  int vertex;
  int weight;
};

struct Vertex {
  std::vector<Edge> connections = std::vector<Edge>();
};

class SimpleGraph {
 public:
  explicit SimpleGraph(int n) {
    this->table = std::vector<Vertex>();
    table.resize(n + 1);
    for (int i = 0; i < n; ++i) {
      table.at(i) = Vertex{};
    }
    size = n;
  }
  int size;
  void add_route(int from, int to, int weight);
  Vertex at(int value) const {
    assert(value != 0);
    return table[value];
  }
 private:
  std::vector<Vertex> table;
};

void SimpleGraph::add_route(int from, int to, int weight) {
  assert(to != 0);
  assert(from != 0);
  table[from].connections.push_back(Edge{to, weight});
  table[to].connections.push_back(Edge{from, weight});
}

int generate_random_start_point(int max) {
  int min = 1;
  return min + (rand() % static_cast<int>(max - min + 1));
}

int get_mst_weight(const SimpleGraph &graph) {
  auto comparer = [](std::pair<int, int> one, std::pair<int, int> another) { return another.first < one.first; };
  auto q = std::priority_queue<std::pair<int, int>,
                               std::vector<std::pair<int, int>>,
                               std::function<bool(std::pair<int, int>, std::pair<int, int>)>>(comparer);
  auto visited = new bool[graph.size + 1]{false};
  auto weights = new int[graph.size + 1]{INT32_MAX};
  q.push(std::make_pair(0, generate_random_start_point(graph.size)));
  int mst_weight = 0;
  while (!q.empty()) {
    auto curr_pair = q.top();
    q.pop();
    int dist = curr_pair.first;
    int curr_vertex = curr_pair.second;
    // если из кучи достали вершину с весом ребра, значение которого больше чем у нее уже стоит, игнорируем вершину
    if (visited[curr_vertex] or (dist > weights[curr_vertex])) continue;
    mst_weight += dist;
    visited[curr_vertex] = true;
    for (auto edge : graph.at(curr_vertex).connections) {
      if (!visited[edge.vertex] and (edge.weight < weights[edge.vertex])) {
        // при обходе графа приоритетными считаются рёбра с меньшим весом
        weights[edge.vertex] = edge.weight;
        q.push(std::make_pair(edge.weight, edge.vertex));
      }
    }
  }
  delete[] visited;
  delete [] weights;
  return mst_weight;
}

int main() {
  int n_vertexes, n_edges;
  std::cin >> n_vertexes >> n_edges;
  auto graph = SimpleGraph(n_vertexes);
  for (int i = 0; i < n_edges; ++i) {
    int from, to, weight;
    std::cin >> from >> to >> weight;
    graph.add_route(from, to, weight);
  }
  std::cout << get_mst_weight(graph);
  return 0;
}
