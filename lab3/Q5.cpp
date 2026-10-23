// Given an m×n matrix where each row and each column is sorted in ascending order. Find the k-th smallest element in this matrix with running time strictly less than O(n2).
// Matrix Properties:
// The matrix is sorted in non-decreasing order both row-wise and column wise.
// The size of the matrix is m×n, where m and n are positive integers. k is a positive integer where 1 ≤ k ≤ m×n.
// Note: Solve this problem using binary search approach
// Example:
// Input:
// Matrix:
// 1 2 2
// 3 4 5
// 4 5 7
// k = 3
// Output:
// 2



#include <bits/stdc++.h>
using namespace std;

// it finds How many entries in the matrix are <= x ?
int countLessEqual(const vector<vector<int>> &a, int x) {
    int m = a.size(), n = a[0].size();
    int count = 0;
    int r = m - 1, c = 0;          // start bottom-left
    while (r >= 0 && c < n) {
        if (a[r][c] <= x) {
            count += r + 1;        // as everything above in this column also fits
            c++;                   // it slide right to the next column
        } else {
            r--;                   // as too big, go up a row
        }
    }
    return count;
}
 
int kthSmallest(const vector<vector<int>> &a, int k) {
    int m = a.size(), n = a[0].size();
    int lo = a[0][0];              // smallest value in the matrix
    int hi = a[m - 1][n - 1];      // largest value in the matrix
 
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (countLessEqual(a, mid) >= k) {
            hi = mid;              // answer is mid or something smaller
        } else {
            lo = mid + 1;          // answer must be bigger than mid
        }
    }
    return lo;
}
 
int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
 
    int m, n;
    cout << "Enter rows and columns: ";
    cin >> m >> n;
    if (m <= 0 || n <= 0) {
        cout << "Matrix dimensions must be positive" << endl;
        return 1;
    }
 
   
    vector<vector<int>> a(m, vector<int>(n));
    cout << "Enter the matrix (rows and columns sorted ascending):" << endl;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            cin >> a[i][j];
 
    int k;
    cout << "Enter k (1 to " << m * n << "): ";
    cin >> k;
    if (k < 1 || k > m * n) {
        cout << "k is out of range" << endl;
        return 1;
    }
 
    cout << "The " << k << "-th smallest element is " << kthSmallest(a, k) << endl;
    return 0;
}