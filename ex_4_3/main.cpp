/*
 Даны неотрицательные целые числа n, k и массив целых чисел из диапазона [0..10^9] размера n.
 Требуется найти k-ю порядковую статистику. т.е. напечатать число, которое бы стояло на позиции с индексом
 k ∈[0..n-1] в отсортированном массиве. Напишите нерекурсивный алгоритм.
 Требования к дополнительной памяти: O(n).
 Требуемое среднее время работы: O(n).
 Функцию Partition следует реализовывать методом прохода двумя итераторами в одном направлении.
 */
#include <iostream>

// функция возвращает место опорного элмента в измененном массиве
template<typename T, typename CompareT>
int partition(T *arr, int start, int stop, int pivot_index, CompareT compare) {
  std::swap(arr[stop - 1], arr[pivot_index]);
  // i указывает на первый элемент больше опорного
  // j следующий не проверенный элемент
  int i = start;
  while (compare(arr[i], arr[stop - 1])) {
    ++i;
  }
  int j = i + 1;
  while (j < stop - 1) {
    if (compare(arr[stop - 1], arr[j])) {
      ++j;
    } else {
      std::swap(arr[i], arr[j]);
      ++i;
      ++j;
    }
  }
  std::swap(arr[i], arr[stop - 1]);
  return i;
}

template<typename T, typename CompareT>
T get_k_order_statistic(int n, int k, T *arr, CompareT cmp) {
  int start = 0;
  int end = n;
  int pivot_place = -1;
  T pivot_element;
  while (pivot_place != k) {
    // рандомно выбираем опорный элемент
    int pivot = start + (rand() % (end - start));
    pivot_element = arr[pivot];
    pivot_place = partition(arr, start, end, pivot, cmp);
    if (k < pivot_place) {
      end = pivot_place;
    } else if (k > pivot_place) {
      start = pivot_place + 1;
    }
  }
  return pivot_element;
}

// Компаратор
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
  int n, k;
  std::cin >> n >> k;
  int *arr = new int[n];

  for (int i = 0; i < n; ++i) {
    std::cin >> arr[i];
  }

  int k_order_statistic = get_k_order_statistic(n, k, arr, ComparerLess<int>());
  std::cout << k_order_statistic;
  delete[] arr;
}
