// Given an m×n matrix where each row and each column is sorted in ascending order. Find the k-th smallest element in this matrix with running time strictly less than O(n2).
// Matrix Properties:
// The matrix is sorted in non-decreasing order both row-wise and column wise.
// The size of the matrix is m×n, where m and n are positive integers. k is a positive integer where 1 ≤ k ≤ m×n.
// Note: Solve this problem using binary search approach
// Example:
// Input:
// Matrix:
// 1 2 2
// 3 4 5
// 4 5 7
// k = 3
// Output:
// 2



#include <bits/stdc++.h>
using namespace std;

bool binarySearchMatrix(int* arr, int row , int col , int find){
    int low = 0;
    int high = (row * col) - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        int midValue = *((arr + (mid / col) * col) + (mid % col)); 

        if (midValue == find) {
            return true;
        } else if (midValue < find) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return false;
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout<<"Enter row, col for matrix : "<<endl;
    int m,n;
    cout<<"row : "<<endl;
    cin>>m;
    cout<<" col : "<<endl;
    cin>>n;
    cout<<endl;
    int arr[m][n];
    int k;
    cout<<"Enter elements for matrix"<<endl;
    for(int i=0;i< m; i++){
        for(int j=0;j< n;j++){
            cin>>arr[i][j];
            cout<<" ";
        }
        cout<<endl;

    }
    cout<<"inter element to search : ";
    cin>>k;
    if(binarySearchMatrix((int*)arr, m, n, k)){
        cout<<"element is "<<k<<" present"<<endl;
    }else{
        cout<<"element is "<<k<<" not present"<<endl;
    }

    return 0;
}