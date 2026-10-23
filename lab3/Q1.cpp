//1. Given a set of strings, find the longest common prefix.
// Example:
// • Input: {”apple”, ”ape”, ”april”}
// • Output: ”ap”

#include <bits/stdc++.h>
using namespace std;
//write for longest common prefix in below function 
string lcp(vector<string> &li){
    if(li.size() == 0) return "";   
    string comp = li[0];
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
    size_t n;
    cout << "Enter the number of strings: ";
    cin >> n;

    vector<string> li(n);
    cout << "Enter the strings: ";
    for(string &word : li){
        cin >> word;
    }

    string ans = lcp(li);
    cout << "Longest common prefix: " << ans;
    return 0;

}