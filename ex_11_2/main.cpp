/*
 Дан невзвешенный неориентированный граф. В графе может быть несколько кратчайших путей между какими-то вершинами.
 Найдите количество различных кратчайших путей между заданными вершинами. Требуемая сложность O(V+E).
*/
#include <iostream>
#include <vector>
#include <queue>
#include <set>

struct Node {
  // количество путей в вершину, при обходе в ширину будет
  // являться количеством кротчайших путей
  int n_paths;
  // длина пути в текущую вершину
  int path_len;
  std::vector<int> neighbors;
};

class SimpleGraph {
 public:
  explicit SimpleGraph(int n) {
    this->table = new std::vector<Node>();
    table->resize(n);
    for (int i = 0; i < n; ++i) {
      table->at(i) = Node{0, 0, std::vector<int>()};
    }
  }
  ~SimpleGraph() {
    delete table;
  }
  SimpleGraph(const SimpleGraph &) = delete;
  SimpleGraph(SimpleGraph &&) = delete;
  SimpleGraph &operator=(const SimpleGraph &) = delete;
  SimpleGraph &operator=(SimpleGraph &&) = delete;
  void add_route(int from, int to);
  Node *at(int value) const;
 private:
  std::vector<Node> *table;
};

void SimpleGraph::add_route(int from, int to) {
  table->at(from).neighbors.push_back(to);
  table->at(to).neighbors.push_back(from);
}
Node *SimpleGraph::at(int value) const {
  return &table->at(value);
}

int shortest_paths_quantity(SimpleGraph *graph, int from, int to) {
  auto q = std::queue<int>();
  auto visited = std::set<int>();
  graph->at(from)->path_len = 0;
  graph->at(from)->n_paths = 1;
  visited.insert(from);
  q.push(from);
  int curr_element;
  do {
    curr_element = q.front();
    q.pop();

    for (int vertex: graph->at(curr_element)->neighbors) {
      if (!visited.count(vertex)) {
        q.push(vertex);
        // длина пути до потомков больше длины путей до родителей на 1
        graph->at(vertex)->path_len = graph->at(curr_element)->path_len + 1;
        visited.insert(vertex);
      } else {
        // если дочерняя вершина уже посещена и находится на уровень выше текущей,
        // то добавляем количество путей до неё к количеству путей до текущей вершины
        if (graph->at(vertex)->path_len < graph->at(curr_element)->path_len) {
          graph->at(curr_element)->n_paths += graph->at(vertex)->n_paths;
        }
      }
    }
  } while (curr_element != to and !q.empty());
  return graph->at(to)->n_paths;
}

int main() {
  int n_vertexes, n_edges;
  std::cin >> n_vertexes >> n_edges;
  auto graph = new SimpleGraph(n_vertexes);
  for (int i = 0; i < n_edges; ++i) {
    int from, to;
    std::cin >> from >> to;
    graph->add_route(from, to);
  }

  int from, to;
  std::cin >> from >> to;
  std::cout << shortest_paths_quantity(graph, from, to);
  delete graph;
  return 0;
}
