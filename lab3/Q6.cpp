// Problem 6: For each element, count smaller elements to its right
//
// Principle used: Divide and Conquer (merge sort on indices)
//
// Idea:
//   Same trick as counting inversions, but we need the count PER element.
//   So we don't move the values around, we sort an array of INDICES by the
//   value they point to. That way we always know whose count to update.
//
//   While merging the left half and the right half (both already sorted):
//       - when we take an element from the right half, remember that one
//         more right element has been placed
//       - when we take an element from the left half, every right element
//         already placed was strictly smaller and sits to its right, so we
//         add that number to this element's answer
//   We take from the left on ties, so only STRICTLY smaller values get counted.
//
// Time:  T(n) = 2T(n/2) + O(n) -> O(n log n)
// Space: O(n)

#include <iostream>
#include <vector>
using namespace std;

void mergeStep(const vector<int> &arr, vector<int> &idx, vector<int> &temp,
               vector<int> &result, int lo, int mid, int hi) {
    int i = lo;
    int j = mid + 1;
    int k = lo;
    int rightTaken = 0;   // how many right-half elements were placed so far

    while (i <= mid && j <= hi) {
        if (arr[idx[i]] <= arr[idx[j]]) {
            // left element goes first, all the right ones placed before it were smaller
            result[idx[i]] += rightTaken;
            temp[k++] = idx[i++];
        } else {
            rightTaken++;
            temp[k++] = idx[j++];
        }
    }

    // left side leftovers are bigger than every right element that was taken
    while (i <= mid) {
        result[idx[i]] += rightTaken;
        temp[k++] = idx[i++];
    }
    while (j <= hi) {
        temp[k++] = idx[j++];
    }

    for (int x = lo; x <= hi; x++) idx[x] = temp[x];
}

void sortIndices(const vector<int> &arr, vector<int> &idx, vector<int> &temp,
                 vector<int> &result, int lo, int hi) {
    if (lo >= hi) return;

    int mid = lo + (hi - lo) / 2;
    sortIndices(arr, idx, temp, result, lo, mid);
    sortIndices(arr, idx, temp, result, mid + 1, hi);
    mergeStep(arr, idx, temp, result, lo, mid, hi);
}

vector<int> countSmallerOnRight(const vector<int> &arr) {
    int n = arr.size();
    vector<int> result(n, 0);
    if (n == 0) return result;

    vector<int> idx(n), temp(n);
    for (int i = 0; i < n; i++) idx[i] = i;

    sortIndices(arr, idx, temp, result, 0, n - 1);
    return result;
}

// slow version to double check on small inputs
vector<int> slowVersion(const vector<int> &arr) {
    int n = arr.size();
    vector<int> result(n, 0);
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (arr[j] < arr[i]) result[i]++;
    return result;
}

void printVec(const vector<int> &v) {
    cout << "[";
    for (size_t i = 0; i < v.size(); i++) cout << v[i] << (i + 1 < v.size() ? ", " : "");
    cout << "]";
}

int main() {
    // example from the assignment sheet, expected [4, 0, 2, 0, 0]
    vector<int> arr = {8, 2, 6, 3, 5};
    cout << "Input: arr = "; printVec(arr); cout << "\n";
    cout << "Output: "; printVec(countSmallerOnRight(arr)); cout << "\n\n";

    // random comparison with the slow version (with duplicates)
    srand(2024);
    int bad = 0;
    for (int t = 0; t < 500; t++) {
        int n = rand() % 40 + 1;
        vector<int> a(n);
        for (int i = 0; i < n; i++) a[i] = rand() % 15;
        if (countSmallerOnRight(a) != slowVersion(a)) bad++;
    }
    cout << "Random checks against brute force: " << (bad == 0 ? "all passed" : "FAILED") << "\n";

    return 0;
}
