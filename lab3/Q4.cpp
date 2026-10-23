// Problem 4: k-th smallest element of an unsorted array in O(n) worst case
//
// Principle used: Divide and Conquer (median of medians / BFPRT selection)
//
// Idea:
//   Normal quickselect is O(n) on average but can go O(n^2) with bad pivots.
//   To guarantee linear time we pick the pivot carefully:
//     1. Split the array into groups of 5.
//     2. Find the median of each group (tiny sort, constant time each).
//     3. Recursively find the median of those medians -> that's our pivot.
//     4. Split the array into  < pivot,  == pivot,  > pivot.
//     5. Only recurse into the one part that contains the k-th element.
//
//   The pivot is guaranteed to be bigger than about 30% and smaller than
//   about 30% of the elements, so the part we recurse into has at most
//   ~70% of n elements. That gives
//       T(n) <= T(n/5) + T(7n/10) + O(n)   which solves to  T(n) = O(n)
//   because 1/5 + 7/10 < 1.
//
// Note: k here is 1-based (k = 1 means the smallest element).

#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

// finds the element at 0-based position k if arr was sorted
int select(vector<int> arr, int k) {
    int n = arr.size();

    // small array, just sort it, it's constant work
    if (n <= 5) {
        sort(arr.begin(), arr.end());
        return arr[k];
    }

    // steps 1 and 2: median of every group of 5
    vector<int> medians;
    for (int i = 0; i < n; i += 5) {
        int end = min(i + 5, n);
        sort(arr.begin() + i, arr.begin() + end);
        medians.push_back(arr[i + (end - i - 1) / 2]);
    }

    // step 3: median of the medians becomes the pivot
    int pivot = select(medians, ((int)medians.size() - 1) / 2);

    // step 4: three way split so duplicates don't cause trouble
    vector<int> smaller, equal, bigger;
    for (int i = 0; i < n; i++) {
        if (arr[i] < pivot) smaller.push_back(arr[i]);
        else if (arr[i] == pivot) equal.push_back(arr[i]);
        else bigger.push_back(arr[i]);
    }

    // step 5: go only where the answer lives
    int s = smaller.size();
    int e = equal.size();
    if (k < s) return select(smaller, k);
    if (k < s + e) return pivot;
    return select(bigger, k - s - e);
}

int kthSmallest(const vector<int> &arr, int k) {
    // k is 1-based from the user's point of view
    return select(arr, k - 1);
}

int main() {
    // example from the assignment sheet, expected 10
    vector<int> arr = {7, 10, 4, 3, 20, 15};
    int k = 4;
    cout << "Input: arr = [7, 10, 4, 3, 20, 15], k = " << k << "\n";
    cout << "Output: " << kthSmallest(arr, k) << "\n\n";

    // compare against plain sorting for a bunch of random arrays
    srand(12345);
    int bad = 0;
    for (int t = 0; t < 500; t++) {
        int n = rand() % 60 + 1;
        vector<int> a(n);
        for (int i = 0; i < n; i++) a[i] = rand() % 20;   // lots of duplicates on purpose
        int kk = rand() % n + 1;

        vector<int> sorted = a;
        sort(sorted.begin(), sorted.end());

        if (kthSmallest(a, kk) != sorted[kk - 1]) bad++;
    }
    cout << "Random checks against sorting: " << (bad == 0 ? "all passed" : "FAILED") << "\n";

    return 0;
}
