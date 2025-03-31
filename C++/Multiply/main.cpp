#include <iostream>

void calculte_cortage(int* counts, int* cortege, int number,
                     int depth, int length) {
  if (depth == length) {
    return;
  }

  if ((cortege[depth] + number) % counts[depth] == 0) {
    cortege[depth] = 0;
    calculte_cortage(counts, cortege, number, depth + 1, length);
  } else {
    cortege[depth] += number;
    calculte_cortage(counts, cortege, 0, depth + 1, length);
  }
}

int main(int argc, char *argv[]) {
  size_t cur_size = static_cast<size_t>(argc) - 1;
  int* counts = new int[cur_size];
  size_t length = 0;
  size_t count_of_cortege = 1;

  for (size_t i = 1; i < static_cast<size_t>(argc); ++i) {
    counts[i - 1] = atoi(argv[i]);
    length += counts[i - 1];
    count_of_cortege *= counts[i - 1];
  }

  int* array = new int[length];

  for (size_t i = 0; i < length; ++i) {
    std::cin >> array[i];
  }

  long long sum = 0;
  int *cortege = new int[cur_size];
  std::fill(cortege, cortege + cur_size, 0);

  for (size_t i = 0; i < count_of_cortege; ++i) {
    bool is_norm = true;

    for (size_t j = 0; j < cur_size; ++j) {
      for (size_t k = j + 1; k < cur_size; ++k) {
        if (cortege[j] == cortege[k]) {
          is_norm = false;
          break;
        }
      }

      if (!is_norm) {
        break;
      }
    }

    if (is_norm) {
      long long cur_multi = array[cortege[0]];
      int cur_length = 0;
      for (size_t j = 0; j < cur_size - 1; ++j) {
        cur_length += counts[j];
        cur_multi *= array[cur_length + cortege[j + 1]];
      }
      sum += cur_multi;
    }
    calculte_cortage(counts, cortege, 1, 0, argc - 1);
  }

  std::cout << sum;

  delete[] counts;
  delete[] array;
  delete[] cortege;
  return 0;
}