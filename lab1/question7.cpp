#include <bits/stdc++.h>
using namespace std;

vector<int> solver(const vector<vector<int>> &arr)
{
    vector<int> ans;

    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<tuple<int, int, int>>> pq;
    for (int i = 0; i < static_cast<int>(arr.size()); i++)
    {
        if (!arr[i].empty())
        {
            pq.push(make_tuple(arr[i][0], i, 0));
        }
    }
    while (!pq.empty())
    {
        int value = get<0>(pq.top());
        int arrIdx = get<1>(pq.top());
        int eleIdx = get<2>(pq.top());
        pq.pop();

        ans.push_back(value);
        int nextIdx = eleIdx + 1;

        if (nextIdx < static_cast<int>(arr[arrIdx].size()))
        {
            pq.push(make_tuple(arr[arrIdx][nextIdx], arrIdx, nextIdx));
        }
    }
    return ans;
}

int main()
{
    int k;
    cin >> k;
    if (k < 0)
    {
        return 1;
    }

    vector<vector<int>> arr(k);
    for (int i = 0; i < k; i++)
    {
        int size;
        cin >> size;
        if (size < 0)
        {
            return 1;
        }

        arr[i].resize(size);
        for (int j = 0; j < size; j++)
        {
            cin >> arr[i][j];
        }
    }

    vector<int> ans = solver(arr);
    cout << "Sorted array: ";
    for (int i = 0; i < ans.size(); i++)
    {
        if (i > 0)
        {
            cout << ' ';
        }
        cout << ans[i];
    }
    cout << '\n';
    return 0;
}