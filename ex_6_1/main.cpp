/*
Дано число N < 106 и последовательность целых чисел из [-231..231] длиной N.
Требуется построить бинарное дерево, заданное наивным порядком вставки.
Т.е., при добавлении очередного числа K в дерево с корнем root, если root→Key ≤ K,
то узел K добавляется в правое поддерево root;
иначе в левое поддерево root. Выведите элементы в порядке pre-order (сверху вниз).
Рекурсия запрещена.
*/


#include <iostream>
#include <queue>
#include <functional>

template<typename T>
struct Node {
  explicit Node(T value) {
    this->value = value;
  }
  T value;
  Node<T> *left = nullptr;
  Node<T> *right = nullptr;
};

template<typename T, typename CompareT>
class SimpleTree {
 public:
  explicit SimpleTree(CompareT compare);
  ~SimpleTree();
  void add(T value);
  void print();
 private:
  Node<T> *root = nullptr;
  CompareT compare;
  // что бы не дублировать код обхода дерева при вызове деструктора и вывода в stdout,
  // принимаем на вход функтор, который будет выполнять необходимую работу
  void pre_order(std::function<void(Node<T> *)> action);
};

template<typename T, typename CompareT>
void SimpleTree<T, CompareT>::add(T value) {
  if (root == nullptr) {
    root = new Node<T>(value);
    return;
  }

  auto *q = new std::queue<Node<T> *>;
  q->push(root);
  while (!q->empty()) {
    Node<T> *next_node = q->front();
    q->pop();
    if (this->compare(value, next_node->value)) {
      // если значение меньше текущего, уходим влево и проверяем следующую вершину
      if (next_node->left == nullptr) {
        next_node->left = new Node<T>(value);
      } else {
        q->push(next_node->left);
      }
    } else {
      // иначе уходим вправо
      if (next_node->right == nullptr) {
        next_node->right = new Node<T>(value);
      } else {
        q->push(next_node->right);
      }
    }
  }
  delete q;
}

template<typename T, typename CompareT>
void SimpleTree<T, CompareT>::pre_order(std::function<void(Node<T> *)> action) {
  auto *dq = new std::deque<Node<T> *>;
  dq->push_front(root);
  // идём от корня вниз
  while (!dq->empty()) {
    auto next_element = dq->front();
    dq->pop_front();

    if (next_element->right != nullptr) {
      dq->push_front(next_element->right);
    }

    // первым всегда будет обработан левый элемент
    if (next_element->left != nullptr) {
      dq->push_front(next_element->left);
    }

    action(next_element);
  }
  delete dq;
}

template<typename T, typename CompareT>
SimpleTree<T, CompareT>::SimpleTree(CompareT compare) {
  this->compare = compare;
}

template<typename T, typename CompareT>
SimpleTree<T, CompareT>::~SimpleTree() {
  pre_order([](Node<T> *node) {
    delete node;
  });
}

template<typename T, typename CompareT>
void SimpleTree<T, CompareT>::print() {
  pre_order([](Node<T> *node) {
    std::cout << node->value << ' ';
  });
}

template<typename T>
class ComparerLess {
 public:
  bool operator()(T &first, T &second) const;
};
template<typename T>
bool ComparerLess<T>::operator()(T &first, T &second) const {
  return first < second;
}

int main() {
  auto *tree = new SimpleTree<int, ComparerLess<int>>(ComparerLess<int>());
  int n;
  std::cin >> n;

  for (int i = 0; i < n; ++i) {
    int value;
    std::cin >> value;
    tree->add(value);
  }
  tree->print();
  delete tree;
}