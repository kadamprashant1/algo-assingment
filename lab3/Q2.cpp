// Problem 2: Longest monotonically increasing subsequence in O(n^2)
//
// Principle used: Dynamic Programming
//
// Idea:
//   Let len[i] be the length of the longest increasing subsequence that
//   ENDS at position i. For every i we look at all j < i, and if
//   arr[j] < arr[i] then we can extend the sequence ending at j by one.
//
//       len[i] = 1 + max(len[j])   for all j < i with arr[j] < arr[i]
//
//   We also remember which j gave the best answer (prev[i]) so we can walk
//   backwards at the end and rebuild the actual sequence, not just its length.
//
// Time:  two nested loops -> O(n^2)
// Space: O(n) for len[] and prev[]

#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

vector<int> longestIncreasing(const vector<int> &arr) {
    int n = arr.size();
    if (n == 0) return {};

    vector<int> len(n, 1);     // every element alone is a sequence of length 1
    vector<int> prev(n, -1);   // -1 means "nothing before me"

    int bestEnd = 0;           // index where the best sequence ends

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            // strictly increasing, and taking j actually makes it longer
            if (arr[j] < arr[i] && len[j] + 1 > len[i]) {
                len[i] = len[j] + 1;
                prev[i] = j;
            }
        }
        if (len[i] > len[bestEnd]) bestEnd = i;
    }

    // walk back through prev[] to collect the sequence (it comes out reversed)
    vector<int> result;
    for (int i = bestEnd; i != -1; i = prev[i]) {
        result.push_back(arr[i]);
    }
    reverse(result.begin(), result.end());
    return result;
}

void runTest(const vector<int> &arr) {
    cout << "Input:  ";
    for (size_t i = 0; i < arr.size(); i++) cout << arr[i] << (i + 1 < arr.size() ? ", " : "");
    cout << "\n";

    vector<int> result = longestIncreasing(arr);
    cout << "Output: ";
    for (size_t i = 0; i < result.size(); i++) cout << result[i] << (i + 1 < result.size() ? ", " : "");
    cout << "   (length " << result.size() << ")\n\n";
}

int main() {
    // example from the assignment sheet
    runTest({4, 8, 5, 7, 2, 9, 10, 1});

    // some extra checks
    runTest({5, 4, 3, 2, 1});          // strictly decreasing, answer has length 1
    runTest({1, 2, 3, 4, 5});          // already sorted
    runTest({10, 22, 9, 33, 21, 50, 41, 60});
    runTest({7});                       // single element

    return 0;
}
