// changes in code

#include <bits/stdc++.h>
using namespace std;

void findHammingDistance(vector<vector<int>> &arr, vector<vector<int>> &p, int n, int k){
    int minHammingDist = INT_MAX;
    pair<int, int> topleft;

    for(int i=0; i<=n-k; i++){
        for(int j=0; j<=n-k; j++){
            int currHammingDist = 0;
            for(int x=0; x<k; x++){
                for(int y=0; y<k; y++){
                    if(arr[i+x][j+y] != p[x][y]){
                        currHammingDist++;
                    }
                }
            }
            if(currHammingDist < minHammingDist){
                minHammingDist = currHammingDist;
                topleft = {i, j};
            }
        }
    }

    cout<<"Minimum Hamming Distance: "<<minHammingDist<<"\n";
    cout<<"Top-left corner of the submatrix: ("<<topleft.first<<", "<<topleft.second<<")\n";
}


int main(){
    int n; 
    cout<<"enter size of n is : ";
    cin>> n;
    vector<vector<int>> arr(n, vector<int>(n));
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            cin>>arr[i][j];
        }
    }

    int k;
    cout<<"enter k : ";
    cin>>k;
    cout<<"\n";
    vector<vector<int>> p(k, vector<int>(k));
    for(int i=0; i<k; i++){
        for(int j=0; j<k; j++){
            cin>>p[i][j];
        }
    }
    findHammingDistance(arr, p, n, k);

    return 0;
}
