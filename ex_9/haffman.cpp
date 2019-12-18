/*
 * Задача 9. Реализуйте алгоритм построения оптимального префиксного кода Хаффмана.
 * При помощи алгоритма реализуйте две функции для создания архива из одного файла и извлечения файла из архива.
 */
#include <vector>
#include <map>
#include <queue>
#include <functional>
#include <cassert>
#include <cstring>
#include "Huffman.h"

typedef unsigned char byte;

class BitsWriter {
 public:
  void write_bit(bool bit);
  void write_byte(byte byte);
  unsigned int buffer_size();

  std::vector<byte> GetResult();

 private:
  std::vector<byte> buffer_;
  unsigned char accumulator_ = 0;
  int bits_count_ = 0;
};

void BitsWriter::write_bit(bool bit) {
  // Ставим бит в аккумулятор на нужное место
  accumulator_ |= static_cast<byte>(bit) << bits_count_;
  ++bits_count_;
  if (bits_count_ == 8) {
    bits_count_ = 0;
    buffer_.push_back(accumulator_);
    accumulator_ = 0;
  }
}

void BitsWriter::write_byte(byte byte) {
  if (bits_count_ == 0) {
    buffer_.push_back(byte);
  } else {
    accumulator_ |= byte << bits_count_;
    buffer_.push_back(accumulator_);
    accumulator_ = byte >> (8 - bits_count_);
  }
}

std::vector<byte> BitsWriter::GetResult() {
  if (bits_count_ != 0) {
    // Добавляем в буфер аккумулятор, если в нем что-то есть.
    buffer_.push_back(accumulator_);
  }
  buffer_.push_back(static_cast<byte>(bits_count_));
  return std::move(buffer_);
}
unsigned int BitsWriter::buffer_size() {
  return buffer_.size();
}

struct Node {
  explicit Node(byte value, int priority, bool empty) {
    this->value = value;
    this->priority = priority;
    this->empty = empty;
  }
  byte value;
  int priority;
  bool empty;
  Node *left = nullptr;
  Node *right = nullptr;
};

class HaffmanTree {
 public:
  explicit HaffmanTree(std::map<byte, int> *frequency_map);
  ~HaffmanTree();
  HaffmanTree(const HaffmanTree &) = delete;
  HaffmanTree(HaffmanTree &&) = delete;
  HaffmanTree &operator=(const HaffmanTree &) = delete;
  HaffmanTree &operator=(HaffmanTree &&) = delete;
  void pre_order(std::function<void(Node *)> action);
  void fill_codes_map(std::map<byte, std::vector<bool>> *map);
  Node *root = nullptr;
 private:
  static void generate_codes(Node *node, std::vector<bool> code, std::map<byte, std::vector<bool>> *m);
};

HaffmanTree::HaffmanTree(std::map<byte, int> *frequency_map) {
  std::function<bool(Node *, Node *)>
      comparer = [](Node *first, Node *second) { return second->priority < first->priority; };
  std::priority_queue<Node *, std::vector<Node *>, std::function<bool(Node *, Node *)>> pq(comparer);
  std::map<byte, int>::iterator it;

  for (it = frequency_map->begin(); it != frequency_map->end(); it++) {
    pq.push(new Node(it->first, it->second, false));
  }

  while (pq.size() != 1) {
    auto first = pq.top();
    pq.pop();
    auto second = pq.top();
    pq.pop();
    auto new_element = new Node('\0', first->priority + second->priority, true);
    new_element->left = first;
    new_element->right = second;
    pq.push(new_element);
  }

  root = pq.top();
}

void HaffmanTree::generate_codes(Node *node, std::vector<bool> code, std::map<byte, std::vector<bool>> *m) {
  if (!node->empty) {
    m->insert(std::pair<byte, std::vector<bool>>(node->value, code));
  }
  if (node->left != nullptr) {
    std::vector<bool> next_code;
    next_code.assign(code.begin(), code.end());
    next_code.push_back(false);
    generate_codes(node->left, next_code, m);
  }
  if (node->left != nullptr) {
    std::vector<bool> next_code;
    next_code.assign(code.begin(), code.end());
    next_code.push_back(true);
    generate_codes(node->right, next_code, m);
  }
}

void HaffmanTree::fill_codes_map(std::map<byte, std::vector<bool>> *map) {
  assert(map->empty());
  std::vector<bool> init_vector;
  generate_codes(root, init_vector, map);
}

void HaffmanTree::pre_order(std::function<void(Node *)> action) {
  auto dq = std::deque<Node *>();
  dq.push_front(root);
  // идём от корня вниз
  while (!dq.empty()) {
    auto next_element = dq.front();
    dq.pop_front();

    if (next_element->right != nullptr) {
      dq.push_front(next_element->right);
    }

    // первым всегда будет обработан левый элемент
    if (next_element->left != nullptr) {
      dq.push_front(next_element->left);
    }

    action(next_element);
  }
}

HaffmanTree::~HaffmanTree() {
  pre_order([](Node *node) {
    delete node;
  });
}

void serialize_int(int val, BitsWriter *writer) {
  byte bytes[4];
  std::memcpy(bytes, &val, sizeof(val));
  for (byte b: bytes) {
    writer->write_byte(b);
  }
}

static void serialize_frequency_table(std::map<byte, int> *map, BitsWriter *writer) {
  assert(!map->empty());
  std::map<byte, int>::iterator it;
  serialize_int(map->size() * 4 + map->size(), writer);
  for (it = map->begin(); it != map->end(); it++) {
    writer->write_byte(it->first);
    serialize_int(it->second, writer);
  }
}

static int deserialize_int(byte *bytes) {
  int val = 0;
  std::memcpy(&val, bytes, 4);
  return val;
}

static void deserialize_frequency_table(std::vector<byte> *bytes, std::map<byte, int> *map) {
  // проверяем что заполняемый словарь пуст
  assert(map->empty());
  // проверяем что в таблице есть хотя бы одна пара значений
  assert(bytes->size() > 4);
  byte int_bytes[4];
  byte symbol{};
  int i = -1;
  for (byte b: *bytes) {
    if (i == -1) {
      symbol = b;
    } else if (i < 4) {
      int_bytes[i] = b;
    }
    ++i;
    if (i == 4) {
      map->insert(std::pair<byte, int>(symbol, deserialize_int(int_bytes)));
      i = -1;
    }
  }
}

static std::vector<byte> encode_stream(IInputStream &input) {
  byte value;
  std::map<byte, int> frequency_map;
  std::vector<byte> raw_data;

  // считываем данные и заполняем таблицу частот
  while (input.Read(value)) {
    if (frequency_map.find(value) == frequency_map.end()) {
      frequency_map[value] = 0;
    } else {
      frequency_map[value] += 1;
    }
    raw_data.push_back(value);
  }

  auto haffman_tree = new HaffmanTree(&frequency_map);
  auto codes_map = std::map<byte, std::vector<bool>>();
  // для того чтобы каждый раз не генерировать код элемента,
  // создадим словарь кодов для всех символов алфавита
  haffman_tree->fill_codes_map(&codes_map);

  BitsWriter writer;

  // в начале файла запишем табоицу частот
  serialize_frequency_table(&frequency_map, &writer);

  //затем закодированный файл, в последнем байте записано количество бит
  //которое необходимо сичтать из предпоследнего байта
  for (byte byte : raw_data) {
    for (bool bit: codes_map.at(byte)) {
      writer.write_bit(bit);
    }
  }
  delete haffman_tree;

  // если размер сжатых данных больше чем не сжатых, то записываем в файл сырые данные
  if (writer.buffer_size() > raw_data.size()) {
    // выставляем последний байт в 0, затем по нему бдуем определять необходимость декодирования
    raw_data.push_back(static_cast<byte>(false));
    return raw_data;
  } else {
    // если последний байт высталвне в 1 - передаются сжатые данные
    auto result = writer.GetResult();
    result.push_back(static_cast<byte>(true));
    return std::move(result);
  }
}

static void write_stream(std::vector<byte> *bytes, IOutputStream &output) {
  for (byte byte : *bytes) {
    output.Write(byte);
  }
}

void Encode(IInputStream &original, IOutputStream &compressed) {
  auto compressed_data = encode_stream(original);
  write_stream(&compressed_data, compressed);
}

bool read_bit(byte b, int position) {
  return static_cast<bool>(static_cast<byte>(true) & (b >> position));
}

void Decode(IInputStream &compressed, IOutputStream &original) {
  byte value;
  std::vector<byte> raw_data;
  while (compressed.Read(value)) {
    raw_data.push_back(value);
  }

  //определим есть ли необходимость декодировать данные
  bool is_data_compressed = static_cast<bool>(raw_data.back());
  raw_data.pop_back();
  if (is_data_compressed) {
    // получим длину табоицы частот
    byte size[4];
    for (int i = 0; i < 4; ++i) {
      size[i] = raw_data[i];
    }
    int freq_table_size = deserialize_int(size);

    // получим таблицу частот
    std::map<byte, int> frequency_table;
    std::vector<byte> freq_encoded;
    for (int i = 4; i < 4 + freq_table_size; ++i) {
      freq_encoded.push_back(raw_data[i]);
    }
    deserialize_frequency_table(&freq_encoded, &frequency_table);

    auto h_tree = new HaffmanTree(&frequency_table);

    // декодируем сообщение
    int last_n_bits = static_cast<int>(raw_data[raw_data.size() - 1]);
    raw_data.pop_back();
    Node *current_node = h_tree->root;
    for (int i = 4 + freq_table_size; i < raw_data.size(); ++i) {
      int max_bit;
      // определяем сколько бит из байта надо считать
      if (i == raw_data.size() - 1) {
        max_bit = last_n_bits;
      } else {
        max_bit = 8;
      }

      // выполняем побитовое чтение из текущего байта
      for (int j = 0; j < max_bit; ++j) {
        bool current_bit = read_bit(raw_data[i], j);
        // в зависимотси от текущего бита уходим влево или вправо по дереву
        current_bit ? current_node = current_node->right : current_node = current_node->left;

        // как только опредлелили символ снова делаем текущей нодой корень
        if (!current_node->empty) {
          original.Write(current_node->value);
          current_node = h_tree->root;
        }

      }
    }
  } else {
    for (byte b: raw_data) {
      original.Write(b);
    }
  }
}

int main() {
  // Получаем данные, которые нужно закодировать
  vector<vector<byte> > input;

  fillInputs(input);

  // Сжимаем данные
  vector<vector<byte> > compressed;
  compressed.resize(input.size());
  for (unsigned int i = 0; i < input.size(); i++) {
    CInputStream iStream(input[i]);
    COutputStream oStream(compressed[i]);
    Encode(iStream, oStream);
  }

  // Распаковываем сжатые данные и проверяем, что они совпадают с оригиналом
  for (unsigned int i = 0; i < input.size(); i++) {
    vector<byte> output;
    CInputStream iStream(compressed[i]);
    COutputStream oStream(output);
    Decode(iStream, oStream);
    if (!isEqual(input[i], output)) {
      cout << -1;
      return 0;
    }
  }

  // Вычисляем степень сжатия
  cout << (100. * calculateSize(compressed) / calculateSize(input));
}