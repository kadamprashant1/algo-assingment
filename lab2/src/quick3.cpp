#include <bits/stdc++.h>
using namespace std;

long long cnt = 0;

int part(vector<int>& a, int low, int high) {
    int pivot = a[low];
    int i = low;
    for (int j = low + 1; j <= high; j++) {
        cnt++;
        if (a[j] < pivot) {
            i++;
            swap(a[i], a[j]);
        }
    }
    swap(a[low], a[i]);
    return i;
}

int median(vector<int>& a, int low, int high) {
    int mid = (low + high) / 2;
    cnt++;
    if (a[low] < a[mid]) {
        cnt++;
        if (a[mid] < a[high]) {
            return mid;
        }
        cnt++;
        if (a[low] < a[high]) {
            return high;
        }
        return low;
    }
    cnt++;
    if (a[low] < a[high]) {
        return low;
    }
    cnt++;
    if (a[mid] < a[high]) {
        return high;
    }
    return mid;
}

void quick(vector<int>& a, int low, int high) {
    while (low < high) {
        int m = median(a, low, high);
        swap(a[low], a[m]);
        int p = part(a, low, high);
        if (p - low < high - p) {
            quick(a, low, p - 1);
            low = p + 1;
        } else {
            quick(a, p + 1, high);
            high = p - 1;
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
    ofstream out("results/quick3.csv");
    if (!out) {
        cout << "cannot open results/quick3.csv" << endl;
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
                quick(a, 0, a.size() - 1);
                auto en = chrono::steady_clock::now();
                chrono::duration<double, micro> t = en - st;
                sumT += t.count();
                sumC += cnt;
            }
            out << names[k] << "," << n << ",avg," << sumT / 5 << "," << (double)sumC / 5 << "\n";
            cout << "quick3 " << names[k] << " " << n << " done" << endl;
        }
    }
    return 0;
}
