// Implement an algorithm to count the number of inversions in an array using
// the divide and conquer approach.
// An inversion in an array is a pair of indices (i,j) such that i < j and arr[i] >
// arr[j].
// Example:
// Input: arr =  [2,3,8,6,1]
//  Output: 5



#include <bits/stdc++.h>
using namespace std;

long long merge_inversions(vector<int> &arr, int low, int mid, int high) {
    int n1 = mid - low + 1;
    int n2 = high - mid;

    vector<int> left(arr.begin() + low, arr.begin() + mid + 1);
    vector<int> right(arr.begin() + mid + 1, arr.begin() + high + 1);

    int i = 0, j = 0, k = low;
    long long inv_count = 0;

    while (i < n1 && j < n2) {
        if (left[i] <= right[j]) {
            arr[k++] = left[i++];
        } else {
            arr[k++] = right[j++];
            inv_count += (n1 - i);
        }
    }
    while (i < n1) arr[k++] = left[i++];
    while (j < n2) arr[k++] = right[j++];

    return inv_count;
}

long long count_inver(vector<int> &arr, int low, int high) {
    if (high <= low) return 0;

    int mid = low + (high - low) / 2;
    long long inversion = 0;
    inversion += count_inver(arr, low, mid);
    inversion += count_inver(arr, mid + 1, high);
    inversion += merge_inversions(arr, low, mid, high);
    return inversion;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int size;
    cout << "Enter size of array" << endl;
    cin >> size;

    if (size <= 0) {
        cout << "Total inversions: 0" << endl;
        return 0;
    }

    vector<int> arr(size);
    cout << "Enter elements in array" << endl;
    for (int i = 0; i < size; i++) cin >> arr[i];

    cout << "Total inversions: " << count_inver(arr, 0, size - 1) << endl;
    return 0;
}