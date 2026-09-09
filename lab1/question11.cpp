
#include <bits/stdc++.h>
using namespace std;

bool hasCycleFrom(int vertex, const vector<vector<int>> &graph,vector<int> &state){
    state[vertex] = 1;

    for (int neighbour : graph[vertex]){
        if (state[neighbour] == 1){
            return true;
        }
        if (state[neighbour] == 0 && hasCycleFrom(neighbour, graph, state)){
            return true;
        }
    }

    state[vertex] = 2;
    return false;
}

bool containsCycle(const vector<vector<int>> &graph){
    vector<int> state(graph.size(), 0);
    for (int vertex = 0; vertex < static_cast<int>(graph.size()); ++vertex){
        if (state[vertex] == 0 && hasCycleFrom(vertex, graph, state)){
            return true;
        }
    }
    return false;
}

int main(){
    int vertexCount;
    int edgeCount;
    cin >> vertexCount >> edgeCount;

    if (vertexCount < 0 || edgeCount < 0){
        return 1;
    }

    vector<vector<int>> graph(vertexCount);
    for (int edge = 0; edge < edgeCount; ++edge){
        int from;
        int to;
        cin >> from >> to;
        if (from < 0 || from >= vertexCount || to < 0 || to >= vertexCount){
            return 1;
        }
        graph[from].push_back(to);
    }

    cout << (containsCycle(graph) ? "Graph contains a cycle\n" : "Graph does not contain a cycle\n");
    return 0;
}
