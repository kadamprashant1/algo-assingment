#include <bits/stdc++.h>
#include <chrono>
using namespace std;

// Insertion Sort
void insertionSort(vector<int>& arr) {

    int n = static_cast<int>(arr.size());

    for (int i = 1; i < n; i++) {

        int key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
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


    // Insertion Sort - Random
    auto start1 = chrono::high_resolution_clock::now();

    insertionSort(randomArr);

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

    cout << "Insertion Sort time: "
         << elapsed1.count()
         << " seconds" << endl;

    cout << "Standard library sort time: "
         << elapsed2.count()
         << " seconds" << endl;


    // Save Insertion Sort result
    ofstream fout1("InsertionSorted_random.txt");

    if (!fout1) {
        cerr << "Error: Could not create InsertionSorted_random.txt"
             << endl;
        return 1;
    }

    for (int value : randomArr) {
        fout1 << value << '\n';
    }

    fout1.close();

    cout << "Output file: InsertionSorted_random.txt" << endl;


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


    // Insertion Sort - Ascending
    auto start3 = chrono::high_resolution_clock::now();

    insertionSort(ascendingArr);

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

    cout << "Insertion Sort time: "
         << elapsed3.count()
         << " seconds" << endl;

    cout << "Standard library sort time: "
         << elapsed4.count()
         << " seconds" << endl;


    // Save Insertion Sort result
    ofstream fout2("InsertionSorted_ascending.txt");

    if (!fout2) {
        cerr << "Error: Could not create InsertionSorted_ascending.txt"
             << endl;
        return 1;
    }

    for (int value : ascendingArr) {
        fout2 << value << '\n';
    }

    fout2.close();

    cout << "Output file: InsertionSorted_ascending.txt" << endl;


    // =====================================================
    // DESCENDING SORTED ARRAY
    // =====================================================

    ifstream fin3("numbers_sorted_reversed.txt");

    if (!fin3) {
        cerr << "Error: Could not open numbers_sorted_reversed.txt" << endl;
        return 1;
    }

    vector<int> descendingArr;

    while (fin3 >> x) {
        descendingArr.push_back(x);
    }

    fin3.close();

    vector<int> descendingArr2 = descendingArr;


    // Insertion Sort - Descending
    auto start5 = chrono::high_resolution_clock::now();

    insertionSort(descendingArr);

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

    cout << "Insertion Sort time: "
         << elapsed5.count()
         << " seconds" << endl;

    cout << "Standard library sort time: "
         << elapsed6.count()
         << " seconds" << endl;


    // Save Insertion Sort result
    ofstream fout3("InsertionSorted_descending.txt");

    if (!fout3) {
        cerr << "Error: Could not create InsertionSorted_descending.txt"
             << endl;
        return 1;
    }

    for (int value : descendingArr) {
        fout3 << value << '\n';
    }

    fout3.close();

    cout << "Output file: InsertionSorted_descending.txt" << endl;

    cout << endl;
    cout << "========================================" << endl;


    return 0;
}