#include "heap_sort.h"
#include <utility>

static void siftDown(std::vector<int>& a, int n, int i, uint64_t& comps) {
    while (true) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n) {
            ++comps;  // key comparison: a[left] > a[largest]
            if (a[left] > a[largest]) {
                largest = left;
            }
        }
        if (right < n) {
            ++comps;  // key comparison: a[right] > a[largest]
            if (a[right] > a[largest]) {
                largest = right;
            }
        }

        if (largest == i) break;

        std::swap(a[i], a[largest]);
        i = largest;
    }
}

void heapSort(std::vector<int>& a, uint64_t& comps) {
    int n = (int)a.size();
    for (int i = n / 2 - 1; i >= 0; --i) {
        siftDown(a, n, i, comps);
    }
    for (int i = n - 1; i > 0; --i) {
        std::swap(a[0], a[i]);
        siftDown(a, i, 0, comps);
    }
}
