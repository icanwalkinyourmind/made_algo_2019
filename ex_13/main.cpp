/*
 Написать алгоритм для решения игры в “пятнашки”. Решением задачи является приведение к виду:
 [ 1 2 3 4 ] [ 5 6 7 8 ] [ 9 10 11 12] [ 13 14 15 0 ], где 0 задает пустую ячейку.
 Достаточно найти хотя бы какое-то решение. Число перемещений костяшек не обязано быть минимальным.
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <functional>
#include <cassert>

const size_t HashParameter = 37;

struct position {
  std::vector<char> chips;
  char zero_place;

  bool is_finish() const;
  bool is_correct() const;
  std::vector<position> siblings() const;

  bool operator==(const position &other) const;
  bool operator!=(const position &other) const { return !operator==(other); }
  int count_evristic();
};

namespace std {
template<>
struct hash<position> {
  size_t operator()(const position &key) const {
    size_t value = 0;
    for (auto chip : key.chips) {
      value = HashParameter * value + static_cast<size_t>(chip);
    }
    return value;
  }
};
}

const position finish_position{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0}, 15};

char get_move_symbol(const position &from, const position &to) {
  char zero_diff = to.zero_place - from.zero_place;
  switch (zero_diff) {
    case 1:return 'L'; // Ноль вправо -> фишка влево
    case -1:return 'R';
    case 4:return 'U';
    case -4:return 'D';
    default: assert(false);
  }
  return 0;
}

bool position::is_finish() const {
  return operator==(finish_position);
}

std::vector<position> position::siblings() const {
  std::vector<position> result;
  if (zero_place < 12) {
    position sibling = *this;
    std::swap(sibling.chips[zero_place], sibling.chips[zero_place + 4]);
    sibling.zero_place += 4;
    result.emplace_back(sibling);
  }
  if (zero_place >= 4) {
    position sibling = *this;
    std::swap(sibling.chips[zero_place], sibling.chips[zero_place - 4]);
    sibling.zero_place -= 4;
    result.emplace_back(sibling);
  }
  if (zero_place % 4 != 0) {
    position sibling = *this;
    std::swap(sibling.chips[zero_place], sibling.chips[zero_place - 1]);
    sibling.zero_place -= 1;
    result.emplace_back(sibling);
  }
  if (zero_place % 4 != 3) {
    position sibling = *this;
    std::swap(sibling.chips[zero_place], sibling.chips[zero_place + 1]);
    sibling.zero_place += 1;
    result.emplace_back(sibling);
  }
  return result;
}

bool position::operator==(const position &other) const {
  for (size_t i = 0; i < chips.size(); ++i) {
    if (chips[i] != other.chips[i]) return false;
  }
  return true;
}
int position::count_evristic() {
  int evristic = 0;
  for (int i = 0; i < 16; i++) {
    if (chips[i] == 0) continue;
    // как эвристику используем манхэтанское расстояние
    evristic += 10 * (abs(i % 4 - (chips[i] - 1) % 4) + abs(i / 4 - (chips[i] - 1) / 4));
  }
  return evristic;
}

bool position::is_correct() const {
  //проверяем существует ли решение
  int inversions = 0;
  for (int i = 0; i < 16; i++)
    for (int j = i + 1; j < 16; j++)
      if (chips[i] != 0 && chips[j] != 0 && chips[i] > chips[j])
        inversions++;
  return (inversions % 2 == 0) == (zero_place / 4 % 2 == 1);
}

bool a_star(const position &start, std::unordered_map<position, position> &parents) {
  auto comparer =
      [](std::pair<int, position> one, std::pair<int, position> another) { return another.first < one.first; };
  auto positions_queue = std::priority_queue<std::pair<int, position>,
                                             std::vector<std::pair<int, position>>,
                                             std::function<bool(std::pair<int, position>, std::pair<int, position>)>>(
      comparer);
  std::unordered_map<position, int> distnaces;
  positions_queue.push(std::make_pair(0, start));
  distnaces[start] = 0;
  while (!positions_queue.empty()) {
    auto current_pair = positions_queue.top();
    auto current = current_pair.second;
    positions_queue.pop();

    for (auto sibling : current.siblings()) {
      // рассчитываем текущее расстояние от начальной позиции
      auto dist = distnaces[current] + 1;
      // если ещё не были в этой позиции или расстояние по этому пути меньше
      // добавляем позицию в очередь с соответсвтующим приоритетом
      if (!distnaces.count(sibling) || dist < distnaces[sibling]) {
        distnaces[sibling] = dist;
        int priority = dist + sibling.count_evristic();
        parents[sibling] = current;
        if (sibling.is_finish()) {
          return true;
        }
        positions_queue.push(std::make_pair(priority, sibling));
      }
    }
  }
  return false;
}

std::vector<char> get_path(const position &start,
                           const std::unordered_map<position, position> &parents) {
  std::vector<char> result;

  position current = finish_position;
  while (current != start) {
    const position &parent = parents.at(current);
    result.push_back(get_move_symbol(parent, current));
    current = parent;
  }
  // Развернем result, так как собирали его с конца.
  std::reverse(result.begin(), result.end());
  return result;
}

std::pair<bool, std::vector<char>> solve_barley_break(const position &start) {
  // Запустим BFS, запоминая предков всех пройденных позиций.
  std::unordered_map<position, position> parents;
  if (!a_star(start, parents)) {
    return std::make_pair(false, std::vector<char>());
  }
  return std::make_pair(true, get_path(start, parents));
}

int main() {
  std::vector<char> start;
  char zero_postioion;
  for (int i = 0; i < 16; ++i) {
    int next;
    std::cin >> next;
    if (next == 0) zero_postioion = static_cast<char>(i);
    start.push_back(static_cast<char>(next));
  }
  auto start_position = position{start, zero_postioion};

  if (!start_position.is_correct()) {
    std::cout << -1;
    return 0;
  }

  const auto result = solve_barley_break(start_position);

  if (result.first) {
    std::cout << result.second.size() << std::endl;
    for (const auto move : result.second) {
      std::cout << move;
    }
  } else {
    std::cout << -1;
  }
  return 0;
}
