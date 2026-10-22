#include "bubble_sort.h"
#include <utility>

void bubbleSort(std::vector<int>& a, uint64_t& comps) {
    int n = (int)a.size();
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            ++comps;  // key comparison: a[j] > a[j+1]
            if (a[j] > a[j + 1]) {
                std::swap(a[j], a[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;  // optimisation: early exit if sorted
    }
}
