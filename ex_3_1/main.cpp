/*Дана очень длинная последовательность целых чисел длины n.
  Требуется вывести в отсортированном виде её наименьшие k элементов.
  Последовательность может не помещаться в память.
  Время работы O(n * log(k)). Доп. память O(k). Использовать слияние. */
#include <iostream>

void merge(int *arr, int start, int mid, int end) {
  if (start == end) {
    return;
  }
  int i = start;
  int j = mid;
  int k = 0;
  int *result = new int[end - start];


  while ((i < mid) and (j < end)) {
    if (arr[i] < arr[j]) {
      result[k] = arr[i];
      ++i;
    } else {
      result[k] = arr[j];
      ++j;
    }
    ++k;
  }

  while (i < mid) {
    result[k] = arr[i];
    ++i;
    ++k;
  }

  while (j < end) {
    result[k] = arr[j];
    ++j;
    ++k;
  }

  for (int m = start; m < end; ++m) {
    arr[m] = result[m - start];
  }
  delete[] result;
}

// k - указывает на начало части массива, которая будет отсортирована
void merge_sort(int *arr, int n, int k) {
  for (int i = 1; i < n; i *= 2) {
    for (int j = k; j < n - i; j += 2 * i) {
      int end = j + 2 * i;
      if (end > n) {
        end = n;
      }
      merge(arr, j, j + i, end);
    }
  }
}

void print_array(int *arr, int n) {
  for (int i = 0; i < n; ++i) {
    std::cout << arr[i] << ' ';
  }
}

int main() {
  int n, k;
  std::cin >> n >> k;
  int arr_size = 2 * k;
  int *input_array = new int[arr_size];
  int sequence_length = 0;

  for (int i = 0; i < k; ++i) {
    std::cin >> input_array[i];
    ++sequence_length;
  }
  merge_sort(input_array, k, 0);

  // пока есть что сичтывать
  while (sequence_length < n) {
    int i;
    // заполняем вторую половину массива
    for (i = k; i < arr_size and sequence_length < n; ++i) {
      std::cin >> input_array[i];
      ++sequence_length;
    }
    // как только заполнится, сортируем её и мержим с первой половиной
    merge_sort(input_array, i, k);
    merge(input_array, 0, k, i);
  }

  print_array(input_array, k);
  delete[] input_array;
  return 0;
}