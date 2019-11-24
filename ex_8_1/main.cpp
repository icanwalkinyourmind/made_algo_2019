/*
 Реализуйте структуру данных типа “множество строк” на основе динамической хеш-таблицы с открытой адресацией.
 Хранимые строки непустые и состоят из строчных латинских букв.
 Хеш-функция строки должна быть реализована с помощью вычисления значения многочлена методом Горнера.
 Начальный размер таблицы должен быть равным 8-ми.
 Перехеширование выполняйте при добавлении элементов в случае, когда коэффициент заполнения таблицы достигает 3/4.
 Структура данных должна поддерживать операции добавления строки в множество,
 удаления строки из множества и проверки принадлежности данной строки множеству.
*/
#include <iostream>
#include <string>
#include <functional>

template<typename T>
struct Node {
  explicit Node(T key) {
    this->key = key;
  }
  T key;
  bool available = true;
};

template<typename T, typename HashT>
class OpenAddressingSet {
 public:
  explicit OpenAddressingSet(HashT hash_function);
  ~OpenAddressingSet();
  OpenAddressingSet(const OpenAddressingSet &) = delete;
  OpenAddressingSet(OpenAddressingSet &&) = delete;
  OpenAddressingSet &operator=(const OpenAddressingSet &) = delete;
  OpenAddressingSet &operator=(OpenAddressingSet &&) = delete;
  bool put(T key);
  bool remove(T key);
  bool contains(T key);
 private:
  const float resize_level = 0.75;
  int table_size = 8;
  int table_capacity = 0;
  Node<T> **table = new Node<T> *[table_size]{};
  unsigned int get_key_position(T key);
  void extend_table();
  HashT hash;
};

template<typename T, typename HashT>
OpenAddressingSet<T, HashT>::OpenAddressingSet(HashT hash_function) {
  hash = hash_function;
}

template<typename T, typename HashT>
OpenAddressingSet<T, HashT>::~OpenAddressingSet() {
  delete[] table;
}

template<typename T, typename HashT>
unsigned int OpenAddressingSet<T, HashT>::get_key_position(T key) {
  int i = 1;
  unsigned int hash_value = hash(key);
  unsigned int index = hash_value % table_size;
  while (table[index] != nullptr && !(table[index]->key == key)) {
    index = (hash_value + i / 2 + i * i / 2) % table_size;
    ++i;
  }
  return index;
}

template<typename T, typename HashT>
void OpenAddressingSet<T, HashT>::extend_table() {
  auto old_table = table;
  int old_table_size = table_size;
  table_size = table_size * 2;
  table = new Node<T> *[table_size]{};

  for (int i = 0; i < old_table_size; ++i) {
    if (old_table[i] != nullptr and old_table[i]->available) {
      table[get_key_position(old_table[i]->key)] = old_table[i];
    }
  }
  delete[] old_table;
}

template<typename T, typename HashT>
bool OpenAddressingSet<T, HashT>::put(T key) {
  if ((float) table_capacity / table_size > resize_level) extend_table();
  int index = get_key_position(key);
  if (table[index] == nullptr or (table[index] != nullptr and !table[index]->available)) {
    table[index] = new Node<T>(key);
    ++table_capacity;
    return true;
  }
  return false;
}

template<typename T, typename HashT>
bool OpenAddressingSet<T, HashT>::remove(T key) {
  int key_position = get_key_position(key);
  if (table[key_position] != nullptr and table[key_position]->available) {
    table[key_position]->available = false;
    return true;
  }
  return false;
}

template<typename T, typename HashT>
bool OpenAddressingSet<T, HashT>::contains(T key) {
  auto element = table[get_key_position(key)];
  return element != nullptr and element->available;
}

void print_result(const std::string &s) {
  std::cout << s << std::endl;
}

int main() {
  auto hash_func = [](const std::string &s) {
    const int a = 64;
    unsigned int hash = 0;
    for (char const c: s) {
      hash = hash * a + (unsigned int) c;
    }
    return hash;
  };

  auto set = new OpenAddressingSet<std::string, std::function<int(std::string)>>(hash_func);
  char operation;
  std::string s;
  while (std::cin >> operation >> s) {
    bool operation_result;
    if (operation == '?') {
      operation_result = set->contains(s);
    } else if (operation == '+') {
      operation_result = set->put(s);
    } else {
      operation_result = set->remove(s);
    }
    operation_result ? print_result("OK") : print_result("FAIL");
  }
  delete set;
}



