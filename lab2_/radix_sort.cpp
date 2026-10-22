#include "radix_sort.h"
#include <algorithm>

void radixSort(std::vector<int>& a, uint64_t& comps) {
    (void)comps;
    int n = (int)a.size();
    if (n <= 1) return;

    int maxVal = *std::max_element(a.begin(), a.end());
    std::vector<int> output(n);

    for (long long exp = 1; maxVal / exp > 0; exp *= 10) {
        int count[10] = {0};
        for (int i = 0; i < n; ++i) {
            count[(a[i] / (int)exp) % 10]++;
        }
        for (int i = 1; i < 10; ++i) {
            count[i] += count[i - 1];
        }
        for (int i = n - 1; i >= 0; --i) {
            int digit = (a[i] / (int)exp) % 10;
            output[count[digit] - 1] = a[i];
            count[digit]--;
        }
        for (int i = 0; i < n; ++i) {
            a[i] = output[i];
        }
    }
}
