#include <bits/stdc++.h>
#include <chrono>
using namespace std;

// Choosing the low element as pivot
void quickSort(vector<int>& arr, int low, int high) {

    if (low < high) {

        int pivot = rand() % (high - low + 1) + low; // Random pivot index

        // Move pivot to the end temporarily
        swap(arr[pivot], arr[high]);

        int i = low - 1;

        for (int j = low; j < high; j++) {

            if (arr[j] < arr[high]) {
                i++;
                swap(arr[i], arr[j]);
            }
        }

        // Put pivot in its correct position
        swap(arr[i + 1], arr[high]);

        int pi = i + 1;

        // Recursively sort left and right subarrays
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main() {

    // Read numbers from numbers_sorted_reversed.txt
    ifstream fin("numbers_sorted_reversed.txt");

    if (!fin) {
        cerr << "Error: Could not open numbers.txt" << endl;
        return 1;
    }

    vector<int> arr;
    int x;

    while (fin >> x) {
        arr.push_back(x);
    }

    fin.close();

    // Make a copy for standard library sort
    vector<int> arr2 = arr;

    // -------------------------------
    // Measure QuickSort
    // -------------------------------
    auto start = chrono::high_resolution_clock::now();

    if (!arr.empty()) {
        quickSort(arr, 0, static_cast<int>(arr.size()) - 1);
    }

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;

    cout << "Elapsed Naive QuickSort time: "
         << elapsed.count()
         << " seconds" << endl;


    // -------------------------------
    // Measure std::sort
    // -------------------------------
    auto start2 = chrono::high_resolution_clock::now();

    sort(arr2.begin(), arr2.end());

    auto end2 = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed2 = end2 - start2;

    cout << "Elapsed Standard library sort time: "
         << elapsed2.count()
         << " seconds" << endl;


    // -------------------------------
    // Output QuickSorted array
    // -------------------------------
    ofstream fout("QuickSorted_numbers_random.txt");

    if (!fout) {
        cerr << "Error: Could not create output file." << endl;
        return 1;
    }

    for (int value : arr) {
        fout << value << '\n';
    }

    fout.close();

    return 0;
}
