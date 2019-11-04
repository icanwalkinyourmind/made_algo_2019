/*
 Даны неотрицательные целые числа n, k и массив целых чисел из диапазона [0..10^9] размера n.
 Требуется найти k-ю порядковую статистику. т.е. напечатать число, которое бы стояло на позиции с индексом
 k ∈[0..n-1] в отсортированном массиве. Напишите нерекурсивный алгоритм.
 Требования к дополнительной памяти: O(n).
 Требуемое среднее время работы: O(n).
 Функцию Partition следует реализовывать методом прохода двумя итераторами в одном направлении.
 */
#include <iostream>

void swap(int *arr, int i, int j) {
  int hold = arr[j];
  arr[j] = arr[i];
  arr[i] = hold;
}

// функция возвращает место опорного элмента в измененном массиве
int partition(int *arr, int start, int stop, int pivot_index) {
  swap(arr, stop - 1, pivot_index);
  // i указывает на первый элемент больше опорного
  // j следующий не проверенный элемент
  int i = start;
  while (arr[i] < arr[stop - 1]) {
    ++i;
  }
  int j = i + 1;
  while (j < stop - 1) {
    if (arr[j] > arr[stop - 1]) {
      ++j;
    } else {
      swap(arr, i, j);
      ++i;
      ++j;
    }
  }
  swap(arr, i, stop - 1);
  return i;
}

int main() {
  int n, k;
  std::cin >> n >> k;
  int *arr = new int[n];

  for (int i = 0; i < n; ++i) {
    std::cin >> arr[i];
  }

  int start = 0;
  int end = n;
  int pivot_place = -1;
  int pivot_element;
  while (pivot_place != k) {
    // рандомно выбираем опорный элемент
    int pivot = start + (rand() % (end - start));
    pivot_element = arr[pivot];
    pivot_place = partition(arr, start, end, pivot);
    if (k < pivot_place) {
      end = pivot_place;
    } else if (k > pivot_place) {
      start = pivot_place + 1;
    }
  }
  std::cout << pivot_element;
  delete[] arr;
}