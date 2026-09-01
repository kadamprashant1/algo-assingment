#include <bits/stdc++.h>
using namespace std;

bool isSafe(vector<vector<int>> &board, int row, int col, int size){
    for(int i=0; i< row ;i++){
        if(board[i][col] == 1) return false;
    }
    for(int i=row-1, j = col-1; i >=0 && j >= 0 ; i--, j--){
        if(board[i][j] == 1) return false;
    }
    for(int i=row-1, j = col+1; i >=0 && j < size ; i--, j++){
        if(board[i][j] == 1) return false;
    }

    return true;
}

void helper(vector<vector<int>> &board, int pos, int size, vector<vector<vector<int>>> &ans){
    if(pos == size){
        ans.push_back(board);
        return;
    }
    for(int col=0; col < size ;col++){
        if(isSafe(board, pos, col, size)){
            board[pos][col] = 1;
            helper(board, pos +1, size, ans);
            board[pos][col] = 0;

        }
    }

}

void printBoard(vector<vector<int>> &board){
    for(auto i: board){
        for(auto j: i) cout<< j<<" ";
        cout<<"\n";
    }
    cout<<"\n";
}

int main(){
    int size;
    cout<<"enter size of board : ";
    cin>>size;
    cout<<"\n";
    vector<vector<vector<int>>> ans;
    vector<vector<int>> board(size, vector<int>(size, 0));
    helper(board, 0, size, ans);
    cout << "Number of solutions : " << ans.size() << "\n\n";
    for(auto &b : ans){
        printBoard(b);
    }

    

}
