#include <bits/stdc++.h>
#include <chrono>
using namespace std;

// Find median of current subarray using Median-of-Medians
int findMedianPivot(vector<int>& arr, int low, int high) {

    int n = high - low + 1;

    // Base case: 5 or fewer elements
    if (n <= 5) {

        vector<int> temp;

        for (int i = low; i <= high; i++) {
            temp.push_back(arr[i]);
        }

        // Sort ONLY these 5 or fewer elements
        sort(temp.begin(), temp.end());

        int pivot = temp[temp.size() / 2];

        // Move the pivot value to arr[high]
        for (int i = low; i <= high; i++) {
            if (arr[i] == pivot) {
                swap(arr[i], arr[high]);
                break;
            }
        }

        return arr[high];
    }

    // Store medians of groups of 5
    vector<int> medians;

    // Divide current subarray into groups of 5
    for (int i = low; i <= high; i += 5) {

        int groupEnd = min(i + 4, high);

        vector<int> group;

        for (int j = i; j <= groupEnd; j++) {
            group.push_back(arr[j]);
        }

        // Sort ONLY this group of at most 5 elements
        sort(group.begin(), group.end());

        // Store median
        medians.push_back(group[group.size() / 2]);
    }

    // Recursively find median of medians
    int pivot = findMedianPivot(
        medians,
        0,
        static_cast<int>(medians.size()) - 1
    );

    // Find pivot in original subarray
    for (int i = low; i <= high; i++) {

        if (arr[i] == pivot) {
            swap(arr[i], arr[high]);
            break;
        }
    }

    return arr[high];
}



void quickSort(vector<int>& arr, int low, int high) {

    if (low < high) {

        int pivot = findMedianPivot(arr, low, high);

        int i = low - 1;

        for (int j = low; j < high; j++) {

            if (arr[j] < pivot) {
                i++;
                swap(arr[i], arr[j]);
            }
        }

        swap(arr[i + 1], arr[high]);

        int pi = i + 1;

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}


int main() {

    ifstream fin("numbers_sorted_reversed.txt");

    
    vector<int> arr;
    int x;

    while (fin >> x) {
        arr.push_back(x);
    }

    fin.close();

    vector<int> arr2 = arr;

    // Median-of-Medians QuickSort
    auto start = chrono::high_resolution_clock::now();

    if (!arr.empty()) {
        quickSort(arr, 0, static_cast<int>(arr.size()) - 1);
    }

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;

    cout << "Elapsed Median-of-Medians QuickSort time: "
         << elapsed.count()
         << " seconds" << endl;


    // Standard library sort
    auto start2 = chrono::high_resolution_clock::now();

    sort(arr2.begin(), arr2.end());

    auto end2 = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed2 = end2 - start2;

    cout << "Elapsed Standard library sort time: "
         << elapsed2.count()
         << " seconds" << endl;


    // Save sorted array
    ofstream fout("QuickSorted_numbers_median.txt");



    for (int value : arr) {
        fout << value << '\n';
    }

    fout.close();

    return 0;
}
