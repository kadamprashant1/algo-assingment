#include "insertion_sort.h"

void insertionSort(std::vector<int>& a, uint64_t& comps) {
    int n = (int)a.size();
    for (int i = 1; i < n; ++i) {
        int key = a[i];
        int j = i - 1;
        // Each iteration of this while-loop performs one key comparison.
        while (j >= 0) {
            ++comps;  // key comparison: a[j] > key
            if (a[j] > key) {
                a[j + 1] = a[j];
                --j;
            } else {
                break;
            }
        }
        a[j + 1] = key;
    }
}
