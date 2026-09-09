#include <bits/stdc++.h>
using namespace std;

void helper(vector<vector<int>> &ans, int n, int k, int nextValue, vector<int> &current)
{
    if (static_cast<int>(current.size()) == k){
        ans.push_back(current);
        return;
    }
    for (int value = nextValue; value <= n; ++value){
        current.push_back(value);
        helper(ans, n, k, value + 1, current);
        current.pop_back();
    }
}

int main()
{
    int n;
    cout << "enter value of n : ";
    cin >> n;
    cout << "\n";
    int k;
    cout << "enter val of k : ";
    cin >> k;
    cout << "\n";

    if (n < 0 || k < 0 || k > n){
        cout << "Invalid input\n";
        return 1;
    }

    vector<vector<int>> ans;
    vector<int> current;
    helper(ans, n, k, 1, current);
    for (const vector<int> &subset : ans){
        cout << "{";
        for (int value : subset)
        {
            cout << value << " ";
        }
        cout << "} ";
    }
    cout << '\n';
    return 0;
}