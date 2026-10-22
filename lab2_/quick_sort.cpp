#include "quick_sort.h"
#include <utility>
#include <random>

static const int PIVOT_SEED = 12345;
static std::mt19937 pivotRng(PIVOT_SEED);

static int lomutoPartition(std::vector<int>& a, int low, int high, uint64_t& comps) {
    int pivot = a[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        ++comps;  // key comparison: a[j] <= pivot
        if (a[j] <= pivot) {
            ++i;
            std::swap(a[i], a[j]);
        }
    }
    std::swap(a[i + 1], a[high]);
    return i + 1;
}

static void quickSortInternal(std::vector<int>& a, int low, int high,
                              uint64_t& comps, int pivotStrategy) {
    while (low < high) {
        int pivotIdx;
        if (pivotStrategy == 0) {
            pivotIdx = low;
        } else if (pivotStrategy == 1) {
            std::uniform_int_distribution<int> dist(low, high);
            pivotIdx = dist(pivotRng);
        } else {
            int mid = low + (high - low) / 2;
            int lo = low, mi = mid, hi2 = high;
            ++comps;
            if (a[lo] > a[mi]) std::swap(lo, mi);
            ++comps;
            if (a[lo] > a[hi2]) std::swap(lo, hi2);
            ++comps;
            if (a[mi] > a[hi2]) std::swap(mi, hi2);
            pivotIdx = mi;
        }

        std::swap(a[pivotIdx], a[high]);
        int p = lomutoPartition(a, low, high, comps);

        if (p - low < high - p) {
            quickSortInternal(a, low, p - 1, comps, pivotStrategy);
            low = p + 1;
        } else {
            quickSortInternal(a, p + 1, high, comps, pivotStrategy);
            high = p - 1;
        }
    }
}

void quickSortFirst(std::vector<int>& a, uint64_t& comps) {
    if (a.size() <= 1) return;
    quickSortInternal(a, 0, (int)a.size() - 1, comps, 0);
}

void quickSortRandom(std::vector<int>& a, uint64_t& comps) {
    if (a.size() <= 1) return;
    pivotRng.seed(PIVOT_SEED);
    quickSortInternal(a, 0, (int)a.size() - 1, comps, 1);
}

void quickSortMedian3(std::vector<int>& a, uint64_t& comps) {
    if (a.size() <= 1) return;
    quickSortInternal(a, 0, (int)a.size() - 1, comps, 2);
}
