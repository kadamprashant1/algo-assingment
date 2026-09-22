#include <bits/stdc++.h>
#include <chrono>
using namespace std;

// Get maximum value
int getMax(vector<int>& arr) {

    int mx = arr[0];

    for (int x : arr) {
        mx = max(mx, x);
    }

    return mx;
}

// Counting Sort for Radix Sort
void countingSort(vector<int>& arr, int exp) {

    int n = arr.size();

    vector<int> output(n);
    int count[10] = {0};

    for (int i = 0; i < n; i++) {
        int digit = (arr[i] / exp) % 10;
        count[digit]++;
    }

    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    for (int i = n - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;

        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }

    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }
}

// Radix Sort
void radixSort(vector<int>& arr) {

    if (arr.empty()) {
        return;
    }

    int mx = getMax(arr);

    for (int exp = 1; mx / exp > 0; exp *= 10) {
        countingSort(arr, exp);
    }
}


int main() {

    // =====================================================
    // RANDOM ARRAY
    // =====================================================

    ifstream fin1("numbers.txt");

    if (!fin1) {
        cerr << "Error: Could not open numbers.txt" << endl;
        return 1;
    }

    vector<int> randomArr;
    int x;

    while (fin1 >> x) {
        randomArr.push_back(x);
    }

    fin1.close();

    vector<int> randomArr2 = randomArr;


    // Radix Sort - Random
    auto start1 = chrono::high_resolution_clock::now();

    radixSort(randomArr);

    auto end1 = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed1 = end1 - start1;


    // std::sort - Random
    auto start2 = chrono::high_resolution_clock::now();

    sort(randomArr2.begin(), randomArr2.end());

    auto end2 = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed2 = end2 - start2;


    cout << "========================================" << endl;
    cout << "RANDOM ARRAY" << endl;
    cout << "========================================" << endl;

    cout << "Radix Sort time: "
         << elapsed1.count()
         << " seconds" << endl;

    cout << "Standard library sort time: "
         << elapsed2.count()
         << " seconds" << endl;


    // Save Radix Sort result
    ofstream fout1("RadixSorted_random.txt");

    if (!fout1) {
        cerr << "Error: Could not create RadixSorted_random.txt"
             << endl;
        return 1;
    }

    for (int value : randomArr) {
        fout1 << value << '\n';
    }

    fout1.close();

    cout << "Output file: RadixSorted_random.txt" << endl;


    // =====================================================
    // ASCENDING SORTED ARRAY
    // =====================================================

    ifstream fin2("numbers_sorted.txt");

    if (!fin2) {
        cerr << "Error: Could not open numbers_sorted.txt" << endl;
        return 1;
    }

    vector<int> ascendingArr;

    while (fin2 >> x) {
        ascendingArr.push_back(x);
    }

    fin2.close();

    vector<int> ascendingArr2 = ascendingArr;


    // Radix Sort - Ascending
    auto start3 = chrono::high_resolution_clock::now();

    radixSort(ascendingArr);

    auto end3 = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed3 = end3 - start3;


    // std::sort - Ascending
    auto start4 = chrono::high_resolution_clock::now();

    sort(ascendingArr2.begin(), ascendingArr2.end());

    auto end4 = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed4 = end4 - start4;


    cout << endl;
    cout << "========================================" << endl;
    cout << "ASCENDING SORTED ARRAY" << endl;
    cout << "========================================" << endl;

    cout << "Radix Sort time: "
         << elapsed3.count()
         << " seconds" << endl;

    cout << "Standard library sort time: "
         << elapsed4.count()
         << " seconds" << endl;


    // Save Radix Sort result
    ofstream fout2("RadixSorted_ascending.txt");

    if (!fout2) {
        cerr << "Error: Could not create RadixSorted_ascending.txt"
             << endl;
        return 1;
    }

    for (int value : ascendingArr) {
        fout2 << value << '\n';
    }

    fout2.close();

    cout << "Output file: RadixSorted_ascending.txt" << endl;


    // =====================================================
    // DESCENDING SORTED ARRAY
    // =====================================================

    ifstream fin3("numbers_sorted_reversed.txt");

    if (!fin3) {
        cerr << "Error: Could not open numbers_sorted_reversed.txt"
             << endl;
        return 1;
    }

    vector<int> descendingArr;

    while (fin3 >> x) {
        descendingArr.push_back(x);
    }

    fin3.close();

    vector<int> descendingArr2 = descendingArr;


    // Radix Sort - Descending
    auto start5 = chrono::high_resolution_clock::now();

    radixSort(descendingArr);

    auto end5 = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed5 = end5 - start5;


    // std::sort - Descending
    auto start6 = chrono::high_resolution_clock::now();

    sort(descendingArr2.begin(), descendingArr2.end());

    auto end6 = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed6 = end6 - start6;


    cout << endl;
    cout << "========================================" << endl;
    cout << "DESCENDING SORTED ARRAY" << endl;
    cout << "========================================" << endl;

    cout << "Radix Sort time: "
         << elapsed5.count()
         << " seconds" << endl;

    cout << "Standard library sort time: "
         << elapsed6.count()
         << " seconds" << endl;


    // Save Radix Sort result
    ofstream fout3("RadixSorted_descending.txt");

    if (!fout3) {
        cerr << "Error: Could not create RadixSorted_descending.txt"
             << endl;
        return 1;
    }

    for (int value : descendingArr) {
        fout3 << value << '\n';
    }

    fout3.close();

    cout << "Output file: RadixSorted_descending.txt" << endl;

    cout << endl;
    cout << "========================================" << endl;


    return 0;
}