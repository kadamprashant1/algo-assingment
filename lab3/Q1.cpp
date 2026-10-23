//1. Given a set of strings, find the longest common prefix.
// Example:
// • Input: {”apple”, ”ape”, ”april”}
// • Output: ”ap”

#include <bits/stdc++.h>
using namespace std;
//write for longest common prefix in below function 
string lcp(vector<string> &li, string comp){
    if(li.size() == 0) return "";   
    comp = li[0];
    for(size_t i = 1; i < li.size(); ++i){
        size_t j = 0;
        while(j < comp.size() && j < li[i].size() && comp[j] == li[i][j]){
            ++j;
        }
        comp.resize(j);
        if(comp.empty()) return "";
    }
    return comp;
}

int main(){
    vector<string> li = {"apple", "ape", "april"};
    string compr = "ap";
    string ans = lcp(li, compr);
    cout<<ans;
    return 0;

}