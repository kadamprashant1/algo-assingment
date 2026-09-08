#include <iostream>
#include <vector>
using namespace std;


void pathsThreeMatrix(const vector<vector<bool>>& adj) {
    int n = adj.size();

    for (int a = 0; a < n; a++)
        for (int b = 0; b < n; b++)
            if (adj[a][b])
                for (int c = 0; c < n; c++)
                    if (adj[b][c])
                        for (int d = 0; d < n; d++)
                            if (adj[c][d])
                                cout << a << " -> " << b << " -> "
                                     << c << " -> " << d << "\n";
}


void pathsThreeList(const vector<vector<int>>& adj) {
    int n = adj.size();

    for (int a = 0; a < n; a++) {
        for (int b : adj[a]) {         
            for (int c : adj[b]) {         
                for (int d : adj[c]) {      
                    cout << a << " -> " << b << " -> "
                         << c << " -> " << d << "\n";
                }
            }
        }
    }
}
int main() {
    int n, m;
    cout << "Enter number of vertices: ";
    cin >> n;

    cout << "Enter number of edges: ";
    cin >> m;

    vector<vector<bool>> adjMatrix(n, vector<bool>(n, false));

    vector<vector<int>> adjList(n);

    cout << "Enter " << m << " edges (u v), vertices numbered 0 to "<< n - 1 << ":\n";

    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;

        if (u < 0 || u >= n || v < 0 || v >= n) {
            cout << "Invalid edge! Vertices must be between 0 and "<< n - 1 << ". Try again.\n";
            i--;                    
            continue;
        }

        adjMatrix[u][v] = adjMatrix[v][u] = true;   
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }

    cout << "\nPaths of length 3 (Adjacency Matrix):\n";
    pathsThreeMatrix(adjMatrix);

    cout << "\nPaths of length 3 (Adjacency List):\n";
    pathsThreeList(adjList);

    return 0;
}

