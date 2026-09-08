#include <bits/stdc++.h>
using namespace std;



int findClosestSubmatrix(const vector<vector<int>> &matrix, const vector<vector<int>> &pattern)
{
    int n = static_cast<int>(matrix.size());
    int k = static_cast<int>(pattern.size());

    if (k == 0)
    {
        return 0;
    }

    int minimumDistance = INT_MAX;

    for (int row = 0; row <= n - k; row++)
    {
        for (int column = 0; column <= n - k; column++)
        {
            int currentDistance = 0;
            for (int patternRow = 0; patternRow < k; patternRow++)
            {
                for (int patternColumn = 0; patternColumn < k; patternColumn++)
                {
                    if (matrix[row + patternRow][column + patternColumn] != pattern[patternRow][patternColumn])
                    {
                        currentDistance++;
                    }
                }
            }
            if (currentDistance < minimumDistance)
            {
                minimumDistance = currentDistance;
            }
        }
    }

    return minimumDistance;
}

int main(){
    int n;
    cin >> n;
    if (n < 0)
    {
        return 1;
    }

    vector<vector<int>> arr(n, vector<int>(n));
    for (int row = 0; row < n; row++)
    {
        for (int column = 0; column < n; column++)
        {
            cin >> arr[row][column];
            if (arr[row][column] != 0 && arr[row][column] != 1)
            {
                return 1;
            }
        }
    }

    int k;
    cin >> k;
    if (k < 0 || k > n){
        return 1;
    }

    vector<vector<int>> p(k, vector<int>(k));
    for (int row = 0; row < k; row++)
    {
        for (int column = 0; column < k; column++)
        {
            cin >> p[row][column];
            if (p[row][column] != 0 && p[row][column] != 1)
            {
                return 1;
            }
        }
    }

    int minimumDistance = findClosestSubmatrix(arr, p);
    cout << "Minimum Hamming Distance: " << minimumDistance << '\n';
    return 0;
}
