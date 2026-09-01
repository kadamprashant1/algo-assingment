#include <bits/stdc++.h>
using namespace std;

void helper(vector<string> &ans, int n, int k, int i, int count, string curr){
    if(count == k){ 
        ans.push_back(curr); return; }
    if(i > n) return;
    helper(ans, n, k, i+1, count+1, curr + to_string(i) + " ");
    helper(ans, n, k, i+1, count, curr);
}


int main(){
    int n; 
    cout<<"enter value of n : ";
    cin>>n;
    cout<<"\n";
    int k;
    cout<<"enter val of k : ";
    cin>>k;
    cout<<"\n";
    vector<string> ans;
    helper(ans,n,k,0 ,0, "");
    for(auto i: ans){
        cout<<"{";
        cout<<i<<" ";
        cout<<"},";
    }
    return 0;
}