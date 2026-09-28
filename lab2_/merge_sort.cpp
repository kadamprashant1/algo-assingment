#include "merge_sort.h"

static void merge(std::vector<int>& a, std::vector<int>& temp,
                  int left, int mid, int right, uint64_t& comps) {
    int i = left, j = mid + 1, k = left;

    while (i <= mid && j <= right) {
        ++comps;  // key comparison: a[i] <= a[j]
        if (a[i] <= a[j]) {
            temp[k++] = a[i++];
        } else {
            temp[k++] = a[j++];
        }
    }
    while (i <= mid) {
        temp[k++] = a[i++];
    }
    while (j <= right) {
        temp[k++] = a[j++];
    }
    for (int idx = left; idx <= right; ++idx) {
        a[idx] = temp[idx];
    }
}

static void mergeSortHelper(std::vector<int>& a, std::vector<int>& temp,
                            int left, int right, uint64_t& comps) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortHelper(a, temp, left, mid, comps);
        mergeSortHelper(a, temp, mid + 1, right, comps);
        merge(a, temp, left, mid, right, comps);
    }
}

void mergeSort(std::vector<int>& a, uint64_t& comps) {
    int n = (int)a.size();
    if (n <= 1) return;
    std::vector<int> temp(n);
    mergeSortHelper(a, temp, 0, n - 1, comps);
}
