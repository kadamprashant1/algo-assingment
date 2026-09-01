// changes in code

#include <bits/stdc++.h>
using namespace std;

vector<int> solver(vector<vector<int>> arr, int k) {
    vector<int> ans;

    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<tuple<int, int, int>>> pq;
    for (int i = 0; i < k; i++) {
        if (!arr[i].empty()) {
            pq.push({arr[i][0], i, 0});
        }
    }
    while (!pq.empty()) {

        auto [value, arrIdx, eleIdx] = pq.top();
        pq.pop();
        ans.push_back(value);
        int nextIdx = eleIdx + 1;

        if (nextIdx < arr[arrIdx].size()) {
            pq.push({arr[arrIdx][nextIdx], arrIdx, nextIdx});
        }
    }
    return ans;
}


int main() {
    int size, k;
    cout << "Enter size: ";
    cin >> size;
    vector<vector<int>> arr(size, vector<int>(size));
    cout << "Enter elements:\n";
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cin >> arr[i][j];
        }
    }
    cout << "Enter k: ";
    cin >> k;
    vector<int> ans = solver(arr, k);
    cout << "Merged array: ";
    for (int i = 0; i < ans.size(); i++) {
        cout << ans[i] << " ";
    }
    return 0;
}
