#include <bits/stdc++.h>
using namespace std;

long long cnt = 0;

void bubble(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            cnt++;
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
}

vector<int> load(string name) {
    ifstream f(name);
    int n = 0;
    f >> n;
    vector<int> a(n);
    for (int i = 0; i < n; i++) {
        f >> a[i];
    }
    return a;
}

int main() {
    string names[3] = {"random", "increasing", "decreasing"};
    vector<int> data[3];
    data[0] = load("input_random.txt");
    data[1] = load("input_increasing.txt");
    data[2] = load("input_decreasing.txt");
    for (int k = 0; k < 3; k++) {
        if (data[k].size() < 100000) {
            cout << "input file missing" << endl;
            return 1;
        }
    }
    int sizes[8] = {100, 500, 1000, 5000, 10000, 20000, 50000, 100000};
    ofstream out("results/bubble.csv");
    if (!out) {
        cout << "cannot open results/bubble.csv" << endl;
        return 1;
    }
    out << fixed << setprecision(3);
    out << "input_type,n,run,time_us,comparisons\n";
    for (int s = 0; s < 8; s++) {
        int n = sizes[s];
        for (int k = 0; k < 3; k++) {
            vector<int> base(data[k].begin(), data[k].begin() + n);
            double sumT = 0;
            long long sumC = 0;
            for (int r = 1; r <= 5; r++) {
                vector<int> a = base;
                cnt = 0;
                auto st = chrono::steady_clock::now();
                bubble(a);
                auto en = chrono::steady_clock::now();
                chrono::duration<double, micro> t = en - st;
                sumT += t.count();
                sumC += cnt;
            }
            out << names[k] << "," << n << ",avg," << sumT / 5 << "," << (double)sumC / 5 << "\n";
            cout << "bubble " << names[k] << " " << n << " done" << endl;
        }
    }
    return 0;
}
