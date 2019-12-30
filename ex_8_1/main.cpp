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
#include <assert.h>

template<typename T, typename HashT>
class OpenAddressingSet {
 public:
  explicit OpenAddressingSet(HashT hash_function, T not_a_value);
  ~OpenAddressingSet();
  OpenAddressingSet(const OpenAddressingSet &) = delete;
  OpenAddressingSet(OpenAddressingSet &&) = delete;
  OpenAddressingSet &operator=(const OpenAddressingSet &) = delete;
  OpenAddressingSet &operator=(OpenAddressingSet &&) = delete;
  bool put(T key);
  bool remove(T key);
  bool contains(T key) const;
 private:
  struct Node {
    T key;
    bool available = false;
  };
  const float resize_level = 0.75;
  const float max_deleted_part = 0.75;
  int table_size = 8;
  int table_capacity = 0;
  int deleted_elements = 0;
  Node *table = new Node[table_size]{};
  unsigned int get_key_position(T key) const;
  void recreate_table(int new_size);
  void extend_table();
  HashT hash;
  unsigned int probe(int i, unsigned int hash_value) const;
  T not_a_value;
};

template<typename T, typename HashT>
OpenAddressingSet<T, HashT>::OpenAddressingSet(HashT hash_function, T not_a_value) {
  hash = hash_function;
  this->not_a_value = not_a_value;
}

template<typename T, typename HashT>
OpenAddressingSet<T, HashT>::~OpenAddressingSet() {
  delete[] table;
}

template<typename T, typename HashT>
unsigned int OpenAddressingSet<T, HashT>::probe(int i, unsigned int hash_value) const {
  return (hash_value + i / 2 + i * i / 2) % table_size;
}

template<typename T, typename HashT>
unsigned int OpenAddressingSet<T, HashT>::get_key_position(T key) const {
  int i = 1;
  unsigned int hash_value = hash(key);
  unsigned int index = hash_value % table_size;
  while (table[index].key != not_a_value && !(table[index].key == key)) {
    index = probe(i, hash_value);
    ++i;
  }
  return index;
}

template<typename T, typename HashT>
void OpenAddressingSet<T, HashT>::recreate_table(int new_size) {
  auto old_table = table;
  int old_table_size = table_size;
  table = new Node[new_size]{};
  for (int i = 0; i < old_table_size; ++i) {
    if (old_table[i].key != not_a_value and old_table[i].available) {
      table[get_key_position(old_table[i].key)] = old_table[i];
    }
  }
  deleted_elements = 0;
  delete[] old_table;
}

template<typename T, typename HashT>
void OpenAddressingSet<T, HashT>::extend_table() {
  recreate_table(table_size * 2);
}

template<typename T, typename HashT>
bool OpenAddressingSet<T, HashT>::put(T key) {
  assert(key != not_a_value);
  if ((float) table_capacity / table_size > resize_level) extend_table();
  int key_position = get_key_position(key);
  if (table[key_position].key != not_a_value and table[key_position].available) {
    // ключ есть в таблце, операция не удалась
    return false;
  } else {
    // если ключа нет или он удлаён, вставляем его на место первого встреченного удалённого элемента, или в пустую ячейку
    int i = 1;
    unsigned int hash_value = hash(key);
    unsigned int index = hash_value % table_size;
    while (table[index].key != not_a_value and table[index].available) {
      index = probe(i, hash_value);
      ++i;
    }
    table[index] = Node{key, true};
    ++table_capacity;
    return true;
  }
}

template<typename T, typename HashT>
bool OpenAddressingSet<T, HashT>::remove(T key) {
  int key_position = get_key_position(key);
  if (table[key_position].key != not_a_value and table[key_position].available) {
    table[key_position].available = false;
    --table_capacity;
    ++deleted_elements;
    // если количество удалённых элементов больше определнного порога, пересоздаём таблицу
    if ((float) deleted_elements / table_size > max_deleted_part) recreate_table(table_size);
    return true;
  }
  return false;
}

template<typename T, typename HashT>
bool OpenAddressingSet<T, HashT>::contains(T key) const {
  auto element = table[get_key_position(key)];
  return element.key != not_a_value and element.available;
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

  auto set = new OpenAddressingSet<std::string, std::function<int(std::string)>>(hash_func, "");
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
  return 0;
}



