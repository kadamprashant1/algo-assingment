// An array A[1...n] is said to have a majority element if more than half of its entries are the same. Given an array, the task is to design an efficient algorithm to tell whether the array has a majority element, and, if so, to find that element. The elements of the array are not necessarily from some ordered domain like the integers, and so there can be no comparisons of the form “is A[i] > A[j]?”. (Think of the array elements as GIF files, say.) However you can answer questions of the form: “is A[i] = A[j]?” in constant time. 
//i) Show how to solve this problem in O(nlogn) time. (Hint: Split the array A into two arrays A1 and A2 of half the size. Does knowing the majorityelements of A1 and A2 help you figure out the majority element of A? If so, you can use a divide-and-conquer approach.) 
// ii) Can you give a linear-time algorithm? (Hint: Here’s another divide-and conquer approach: 
//     • Pair up the elements of A arbitrarily, to get n/2 pairs
//     • Look at each pair: if the two elements are different, discard both of them; if they are the same, keep just one of them. Show that after this procedure there are at most n/2 elements left, and that they have a majority element if and only if A does.)



#include <bits/stdc++.h>
using namespace std;

// Counts occurrences of x in arr[lo..hi]
int countInRange(const vector<int>& arr, int lo, int hi, int x) {
    int c = 0;
    for (int i = lo; i <= hi; i++)
        if (arr[i] == x) c++;
    return c;
}

// ---------- Part (i): Divide and conquer, O(n log n) ----------
// Returns {true, element} if arr[lo..hi] has a majority element, else {false, 0}
pair<bool, int> majorityDC(const vector<int>& arr, int lo, int hi) {
    if (lo == hi) return {true, arr[lo]};

    int mid = lo + (hi - lo) / 2;
    pair<bool, int> L = majorityDC(arr, lo, mid);
    pair<bool, int> R = majorityDC(arr, mid + 1, hi);

    int len = hi - lo + 1;

    // A majority of the whole range must be a majority of at least one half
    if (L.first) {
        if (countInRange(arr, lo, hi, L.second) > len / 2)
            return {true, L.second};
    }
    if (R.first) {
        if (countInRange(arr, lo, hi, R.second) > len / 2)
            return {true, R.second};
    }
    return {false, 0};
}

// ---------- Part (ii): Pairing, O(n) ----------
// Returns a candidate that is the only possible majority element.
// The candidate must still be verified against the original array.
pair<bool, int> candidateLinear(vector<int> a) {
    if (a.empty()) return {false, 0};

    while (a.size() > 1) {
        // Odd size: check the leftover element directly
        if (a.size() % 2 == 1) {
            int x = a.back();
            int c = 0;
            for (int v : a)
                if (v == x) c++;
            if (c > (int)a.size() / 2) return {true, x};
            a.pop_back();   // x is not a majority, so removing it is safe
        }

        // Pair up: different -> discard both, same -> keep one
        vector<int> b;
        for (size_t i = 0; i + 1 < a.size(); i += 2)
            if (a[i] == a[i + 1]) b.push_back(a[i]);

        a = b;
        if (a.empty()) return {false, 0};
    }
    return {true, a[0]};
}

pair<bool, int> majorityLinear(const vector<int>& arr) {
    pair<bool, int> cand = candidateLinear(arr);
    if (!cand.first) return {false, 0};

    // Verify the candidate with one pass
    int c = countInRange(arr, 0, (int)arr.size() - 1, cand.second);
    if (c > (int)arr.size() / 2) return {true, cand.second};
    return {false, 0};
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    cout << "Enter size for array : " << endl;
    int size;
    cin >> size;
    if (size <= 0) {
        cout << "Array must be non-empty" << endl;
        return 1;
    }

    vector<int> arr;
    cout << "Enter element of array: " << endl;
    int temp;
    for (int i = 0; i < size; i++) {
        cin >> temp;
        arr.push_back(temp);
    }

    pair<bool, int> r1 = majorityDC(arr, 0, size - 1);
    cout << "\n(i)  Divide & conquer O(n log n): ";
    if (r1.first) cout << "majority element = " << r1.second << endl;
    else          cout << "no majority element" << endl;

    pair<bool, int> r2 = majorityLinear(arr);
    cout << "(ii) Pairing method O(n):         ";
    if (r2.first) cout << "majority element = " << r2.second << endl;
    else          cout << "no majority element" << endl;

    return 0;
}