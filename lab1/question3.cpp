#include<bits/stdc++.h>
using namespace std;

void generateGreyCode(int n, vector<string> &ans, string curr){
    if(0 == n){
        ans.push_back(curr);
        return;
    }
    generateGreyCode(n-1, ans,curr +"0");
    generateGreyCode(n-1, ans, curr +"1");
}

int main(){
    cout<<"Enter the number of bits for Grey code: ";
    int n;
    cin>>n;
    vector<string> greyCode;
    generateGreyCode(n, greyCode, "");
    for(const string& code : greyCode){
        cout<<code<<" ";
    }
    cout<<endl;
    return 0;
}

