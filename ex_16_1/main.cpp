/*
Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки ­– n. Время O(n + p), доп. память – O(p).
Вариант 1. С помощью префикс-функции (алгоритм Кнута-Морриса-Пратта).
*/
#include <iostream>
#include <vector>

std::vector<int> find_matches(const std::string &substr, const std::string &target) {
  int matches[substr.size()];
  std::vector<int> positions;
  matches[0] = 0;

  // рассчитываем префикс функцию для подстроки
  for (int i = 0; i < substr.size() - 1; ++i) {
    auto prefix = matches[i];
    while (prefix != 0 and substr[i + 1] != substr[prefix]) {
      prefix = matches[prefix - 1];
    }
    if (substr[i + 1] == substr[prefix]) {
      matches[i + 1] = prefix + 1;
    } else {
      matches[i + 1] = 0;
    }
  }

  int prefix = matches[0];
  for (int i = 0; i < target.size(); ++i) {
    while (prefix != 0 and target[i] != substr[prefix]) {
      prefix = matches[prefix - 1];
    }
    if (target[i] == substr[prefix]) {
      ++prefix;
    } else {
      prefix = 0;
    }
    //если длина текущего префикса равна длине подстроки значит получили позицию вхождения
    if (prefix == substr.size()) positions.push_back(i + 1 - substr.size());
  }

  return positions;
}

int main() {
  std::string substr, target;
  std::getline(std::cin, substr);
  std::getline(std::cin, target);
  for (auto pos: find_matches(substr, target)) {
    std::cout << pos << ' ';
  }
  return 0;
}
