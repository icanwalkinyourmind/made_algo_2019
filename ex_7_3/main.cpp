/*
Множество натуральных чисел постоянно меняется.
Элементы в нем добавляются и удаляются по одному.
Вычислите указанные порядковые статистики после каждого добавления или удаления.
Требуемая скорость выполнения запроса - O(log n) в среднем.
В реализации используйте декартовы деревья.
*/
#include <iostream>
#include <deque>
#include <functional>

template<typename T>
struct TreapNode {
  explicit TreapNode(T value) {
    this->value = value;
    priority = random();
  }
  T value;
  int priority;
  int subtree_size = 1;
  TreapNode<T> *left = nullptr;
  TreapNode<T> *right = nullptr;
};

template<typename T, typename CompareT>
class Treap {
 public:
  explicit Treap(CompareT compare);
  ~Treap();
  void add(T value);
  void remove(T value);
  T get_k_stat(int k);
 private:
  TreapNode<T> *root = nullptr;
  TreapNode<T> *merge(TreapNode<T> *left, TreapNode<T> *right);
  std::pair<TreapNode<T> *, TreapNode<T> *> split(TreapNode<T> *node, T value);
  void pre_order(std::function<void(TreapNode<T> *)> action);
  CompareT compare;
  int get_subtree_size(TreapNode<T> *node);
};

template<typename T, typename CompareT>
Treap<T, CompareT>::Treap(CompareT compare) {
  this->compare = compare;
}

template<typename T, typename CompareT>
TreapNode<T> *Treap<T, CompareT>::merge(TreapNode<T> *left, TreapNode<T> *right) {
  if (left == nullptr) return right;
  if (right == nullptr) return left;

  if (left->priority < right->priority) {
    TreapNode<T> *new_root = left;
    new_root->right = merge(new_root->right, right);
    new_root->subtree_size = get_subtree_size(new_root->right) + get_subtree_size(new_root->left) + 1;
    return new_root;
  } else {
    TreapNode<T> *new_root = right;
    new_root->left = merge(left, right->left);
    new_root->subtree_size = get_subtree_size(new_root->right) + get_subtree_size(new_root->left) + 1;
    return new_root;
  }
}

template<typename T, typename CompareT>
std::pair<TreapNode<T> *, TreapNode<T> *> Treap<T, CompareT>::split(TreapNode<T> *node, T value) {
  if (!node) {
    return std::make_pair(nullptr, nullptr);
  }

  if (this->compare(node->value, value)) {
    node->subtree_size -= get_subtree_size(node->right);
    auto right_pair = split(node->right, value);
    node->right = right_pair.first;
    node->subtree_size += get_subtree_size(node->right);
    return std::make_pair(node, right_pair.second);
  } else {
    node->subtree_size -= get_subtree_size(node->left);
    auto left_pair = split(node->left, value);
    node->left = left_pair.second;
    node->subtree_size += get_subtree_size(node->left);
    return std::make_pair(left_pair.first, node);
  }
}

template<typename T, typename CompareT>
void Treap<T, CompareT>::add(T value) {
  auto new_node = new TreapNode<T>(value);
  auto splited = split(root, value);
  root = merge(merge(splited.first, new_node), splited.second);
}

template<typename T, typename CompareT>
void Treap<T, CompareT>::remove(T value) {
  auto splited = split(root, value);
  root = merge(splited.first, split(splited.second, ++value).second);
}

template<typename T, typename CompareT>
T Treap<T, CompareT>::get_k_stat(int k) {
  auto *dq = new std::deque<TreapNode<T> *>;
  T stat;
  dq->push_front(root);
  // рассчитываем статистику для корня
  int current_stat = root->subtree_size - 1 - get_subtree_size(root->right);
  // идём от корня вниз
  while (!dq->empty()) {
    auto current_element = dq->front();
    dq->pop_front();

    if (k < current_stat & current_element->left != nullptr) {
      dq->push_front(current_element->left);
      // если идём влево то надо из текущей статистки вычесть количество элементов в левом поддререве за
      // вычетом элементов его левого потомка
      current_stat = current_stat - (get_subtree_size(dq->front()) - get_subtree_size(dq->front()->left));
    } else if (k > current_stat & current_element->right != nullptr) {
      dq->push_front(current_element->right);
      // если идём вправо то надо к текущей статистке прибавить количество элементов в правом поддререве за
      // вычетом элементов его правого потомка
      current_stat = current_stat + get_subtree_size(dq->front()) - get_subtree_size(dq->front()->right);
    } else {
      return current_element->value;
    }
  }
  delete dq;
}

template<typename T, typename CompareT>
void Treap<T, CompareT>::pre_order(std::function<void(TreapNode<T> *)> action) {
  auto *dq = new std::deque<TreapNode<T> *>;
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
Treap<T, CompareT>::~Treap() {
  pre_order([](TreapNode<T> *node) {
    delete node;
  });
}
template<typename T, typename CompareT>
int Treap<T, CompareT>::get_subtree_size(TreapNode<T> *node) {
  return node == nullptr ? 0 : node->subtree_size;
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
  auto treap = new Treap<int, ComparerLess<int>>(ComparerLess<int>());
  int n;
  std::cin >> n;
  for (int i = 0; i < n; ++i) {
    int value, k;
    std::cin >> value >> k;
    if (value < 0) {
      treap->remove(abs(value));
    } else {
      treap->add(value);
    }
    std::cout << treap->get_k_stat(k) << "\n";
  }
  delete treap;
}