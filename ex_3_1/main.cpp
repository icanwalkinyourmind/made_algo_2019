/*Дана очень длинная последовательность целых чисел длины n.
  Требуется вывести в отсортированном виде её наименьшие k элементов.
  Последовательность может не помещаться в память.
  Время работы O(n * log(k)). Доп. память O(k). Использовать слияние. */
#include <iostream>

template<typename T, typename CompareT>
void merge(T *arr, int start, int mid, int end, CompareT compare) {
  if (start == end) {
    return;
  }
  int i = start;
  int j = mid;
  int k = 0;
  T *result = new T[end - start];

  while ((i < mid) and (j < end)) {
    if (compare(arr[i], arr[j])) {
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
template<typename T, typename CompareT>
void merge_sort(T *arr, int n, int k, CompareT compare) {
  for (int i = 1; i < n; i *= 2) {
    for (int j = k; j < n - i; j += 2 * i) {
      int end = j + 2 * i;
      if (end > n) {
        end = n;
      }
      merge(arr, j, j + i, end, compare);
    }
  }
}

template<typename T>
void print_array(T *arr, int n) {
  for (int i = 0; i < n; ++i) {
    std::cout << arr[i] << ' ';
  }
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
  int n, k;
  std::cin >> n >> k;
  int arr_size = 2 * k;
  int *input_array = new int[arr_size];
  int sequence_length = 0;

  ComparerLess<int> comparer = ComparerLess<int>();

  for (int i = 0; i < k; ++i) {
    std::cin >> input_array[i];
    ++sequence_length;
  }
  merge_sort(input_array, k, 0, comparer);

  // пока есть что сичтывать
  while (sequence_length < n) {
    int i;
    // заполняем вторую половину массива
    for (i = k; i < arr_size and sequence_length < n; ++i) {
      std::cin >> input_array[i];
      ++sequence_length;
    }
    // как только заполнится, сортируем её и мержим с первой половиной
    merge_sort(input_array, i, k, comparer);
    merge(input_array, 0, k, i, comparer);
  }

  print_array(input_array, k);
  delete[] input_array;
  return 0;
}