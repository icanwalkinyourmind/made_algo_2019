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
#include <assert.h>

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
  Treap(const Treap &) = delete;
  Treap(Treap &&) = delete;
  Treap &operator=(const Treap &) = delete;
  Treap &operator=(Treap &&) = delete;
  void add(T value);
  void remove(T value);
  T get_k_stat(int k) const;
 private:
  TreapNode<T> *root = nullptr;
  CompareT compare;
  static TreapNode<T> *merge(TreapNode<T> *left, TreapNode<T> *right);
  static std::pair<TreapNode<T> *, TreapNode<T> *> split(TreapNode<T> *node, T value, CompareT compare_);
  static int get_subtree_size(TreapNode<T> *node);
  static void delete_subtree(TreapNode<T> *node);
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
std::pair<TreapNode<T> *, TreapNode<T> *> Treap<T, CompareT>::split(TreapNode<T> *node, T value, CompareT compare_) {
  if (!node) {
    return std::make_pair(nullptr, nullptr);
  }

  if (compare_(node->value, value)) {
    node->subtree_size -= get_subtree_size(node->right);
    auto right_pair = split(node->right, value, compare_);
    node->right = right_pair.first;
    node->subtree_size += get_subtree_size(node->right);
    return std::make_pair(node, right_pair.second);
  } else {
    node->subtree_size -= get_subtree_size(node->left);
    auto left_pair = split(node->left, value, compare_);
    node->left = left_pair.second;
    node->subtree_size += get_subtree_size(node->left);
    return std::make_pair(left_pair.first, node);
  }
}

template<typename T, typename CompareT>
void Treap<T, CompareT>::add(T value) {
  auto new_node = new TreapNode<T>(value);
  auto splited = split(root, value, compare);
  root = merge(merge(splited.first, new_node), splited.second);
}

template<typename T, typename CompareT>
void Treap<T, CompareT>::remove(T value) {
  auto splited = split(root, value, compare);
  root = merge(splited.first, split(splited.second, ++value, compare).second);
}

template<typename T, typename CompareT>
T Treap<T, CompareT>::get_k_stat(int k) const {
  assert(k >= 0 and k < root->subtree_size);
  // рассчитываем статистику для корня
  int current_stat = root->subtree_size - 1 - get_subtree_size(root->right);
  auto current_element = root;
  // идём от корня вниз
  while (true) {
    if (k < current_stat & current_element->left != nullptr) {
      current_element = current_element->left;
      // если идём влево то надо из текущей статистки вычесть количество элементов в левом поддререве за
      // вычетом элементов его левого потомка
      current_stat = current_stat - (get_subtree_size(current_element) - get_subtree_size(current_element->left));
    } else if (k > current_stat & current_element->right != nullptr) {
      current_element = current_element->right;
      // если идём вправо то надо к текущей статистке прибавить количество элементов в правом поддререве за
      // вычетом элементов его правого потомка
      current_stat = current_stat + get_subtree_size(current_element) - get_subtree_size(current_element->right);
    } else {
      return current_element->value;
    }
  }
}

template<typename T, typename CompareT>
void Treap<T, CompareT>::delete_subtree(TreapNode<T> *node) {
  if (!node) return;
  delete_subtree(node->left);
  delete_subtree(node->right);
  delete node;
}

template<typename T, typename CompareT>
Treap<T, CompareT>::~Treap() {
  delete_subtree(root);
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