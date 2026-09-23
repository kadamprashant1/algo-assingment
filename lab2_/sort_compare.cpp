/*
 * sort_compare.cpp
 * ================
 * Experimental framework for comparing sorting algorithms.
 *
 * Algorithms implemented:
 *   1. Bubble Sort
 *   2. Insertion Sort
 *   3. Merge Sort
 *   4. QuickSort — first-element pivot   (QuickSort_First)
 *   5. QuickSort — random-element pivot   (QuickSort_Random)
 *   6. QuickSort — median-of-three pivot  (QuickSort_Median3)
 *   7. Heap Sort
 *   8. Radix Sort
 *
 * KEY COMPARISON COUNTING CONVENTION
 * -----------------------------------
 * A "key comparison" is any comparison that involves data elements (keys)
 * and determines their relative ordering.  Examples:
 *     a[i] < a[j],  a[i] > a[j],  a[i] <= pivot,  a[j] > pivot
 *
 * The following are NOT counted:
 *   - Loop-index comparisons (i < n, low < high, etc.)
 *   - Comparisons of array indices
 *   - Size / length comparisons
 *   - Random-number comparisons
 *   - Assignments, swaps, arithmetic, function calls
 *
 * For Radix Sort the comparison count is always 0 because it is
 * a non-comparison-based algorithm; it determines ordering by
 * examining individual digits/bits of the keys rather than
 * comparing keys against one another.
 *
 * RANDOM-SEED HANDLING
 * --------------------
 * Two separate random-number generators are used:
 *   - inputRng  : seeded with INPUT_SEED (42) — generates input data
 *   - pivotRng  : seeded with PIVOT_SEED (12345) — used by QuickSort_Random
 * This ensures that consuming random numbers for pivot selection does
 * not change the input data seen by other algorithms.
 *
 * BUBBLE SORT RESTRICTION
 * -----------------------
 * Bubble Sort is only executed when n <= 20000.  For the standard
 * experiment sizes (1000, 5000, 10000, 50000, 100000) this means it
 * runs for n = 1000, 5000, 10000 and is skipped for 50000, 100000.
 *
 * WHY MINIMUM RUNTIME IS USED
 * ----------------------------
 * The minimum across 5 trials is the primary runtime metric because
 * it reduces the influence of OS scheduling and background processes.
 * It represents the closest measurement to the algorithm's true cost
 * on the given hardware.
 *
 * INCREASING / DECREASING / RANDOM INPUTS
 * ----------------------------------------
 * These are fixed input distributions used for experimentation.
 * They are NOT automatically equivalent to theoretical best/worst/
 * average cases for every algorithm.  For example, "increasing" is
 * the best case for Insertion Sort but the worst case for
 * QuickSort_First.
 *
 * Compilation:
 *   g++ -O2 -std=c++11 sort_compare.cpp -o sort_compare
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cassert>
#include <random>
#include <iomanip>
#include <cmath>

using namespace std; // correct accordingly
/* ------------------------------------------------------------------ */
/*                        CONFIGURATION                                */
/* ------------------------------------------------------------------ */

static const int INPUT_SEED = 42;       // Seed for input generation
static const int PIVOT_SEED = 12345;    // Seed for QuickSort_Random pivot
static const int NUM_TRIALS = 5;
static const int BUBBLE_SORT_MAX_N = 20000;

// Experiment sizes
static const int SIZES[] = {1000, 5000, 10000, 50000, 100000};
static const int NUM_SIZES = 5;

// Input type names
static const std::string INPUT_TYPES[] = {"random", "increasing", "decreasing"};
static const int NUM_INPUT_TYPES = 3;

// Algorithm names (as they appear in CSV)
static const std::string ALGO_NAMES[] = {
    "BubbleSort",
    "InsertionSort",
    "MergeSort",
    "QuickSort_First",
    "QuickSort_Random",
    "QuickSort_Median3",
    "HeapSort",
    "RadixSort"
};
static const int NUM_ALGOS = 8;

/* ------------------------------------------------------------------ */
/*                    GLOBAL PIVOT RNG                                  */
/* ------------------------------------------------------------------ */

// This RNG is used exclusively by QuickSort_Random for pivot selection.
// It is re-seeded at the start of each sort call for reproducibility,
// but NOT re-seeded within recursive calls.
static std::mt19937 pivotRng(PIVOT_SEED);

/* ------------------------------------------------------------------ */
/*                    INPUT GENERATION                                  */
/* ------------------------------------------------------------------ */

// Generate input data.  Uses a dedicated RNG seeded deterministically
// so that the same (input_type, n, trial) always produces the same data.
std::vector<int> generateInput(const string& inputType, int n, int trial) {
    std::vector<int> data(n);

    if (inputType == "random") {
        // Seed depends on n and trial so each combination is unique
        // but reproducible.
        std::mt19937 rng(INPUT_SEED + n * 1000 + trial);
        std::uniform_int_distribution<int> dist(0, n * 10);
        for (int i = 0; i < n; ++i) {
            data[i] = dist(rng);
        }
    } else if (inputType == "increasing") {
        for (int i = 0; i < n; ++i) {
            data[i] = i;
        }
    } else if (inputType == "decreasing") {
        for (int i = 0; i < n; ++i) {
            data[i] = n - 1 - i;
        }
    }

    return data;
}

// Read input from a file.  Format: first line is n, then n integers.
std::vector<int> readInputFromFile(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cerr << "Error: Cannot open file '" << filename << "'" << std::endl;
        exit(1);
    }

    int n;
    if (!(fin >> n)) {
        std::cerr << "Error: Cannot read n from file." << std::endl;
        exit(1);
    }

    std::vector<int> data(n);
    for (int i = 0; i < n; ++i) {
        if (!(fin >> data[i])) {
            std::cerr << "Error: Expected " << n << " integers but could only read "
                      << i << "." << std::endl;
            exit(1);
        }
    }

    // Check for extra data
    int extra;
    if (fin >> extra) {
        std::cerr << "Warning: File contains more data than expected (" << n
                  << " integers)." << std::endl;
    }

    fin.close();
    return data;
}

/* ------------------------------------------------------------------ */
/*                    SORTING ALGORITHMS                                */
/* ------------------------------------------------------------------ */

/* --- Bubble Sort --- */
void bubbleSort(std::vector<int>& a, uint64_t& comps) {
    int n = (int)a.size();
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            ++comps;  // key comparison: a[j] > a[j+1]
            if (a[j] > a[j + 1]) {
                std::swap(a[j], a[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;  // optimisation: early exit if sorted
    }
}

/* --- Insertion Sort --- */
void insertionSort(std::vector<int>& a, uint64_t& comps) {
    int n = (int)a.size();
    for (int i = 1; i < n; ++i) {
        int key = a[i];
        int j = i - 1;
        // Each iteration of this while-loop performs one key comparison.
        while (j >= 0) {
            ++comps;  // key comparison: a[j] > key
            if (a[j] > key) {
                a[j + 1] = a[j];
                --j;
            } else {
                break;
            }
        }
        a[j + 1] = key;
    }
}

/* --- Merge Sort --- */
// Merge two sorted halves a[left..mid] and a[mid+1..right]
static void merge(std::vector<int>& a, std::vector<int>& temp,
                  int left, int mid, int right, uint64_t& comps) {
    int i = left, j = mid + 1, k = left;

    while (i <= mid && j <= right) {
        ++comps;  // key comparison: a[i] <= a[j]
        if (a[i] <= a[j]) {
            temp[k++] = a[i++];
        } else {
            temp[k++] = a[j++];
        }
    }
    while (i <= mid) {
        temp[k++] = a[i++];
    }
    while (j <= right) {
        temp[k++] = a[j++];
    }
    for (int idx = left; idx <= right; ++idx) {
        a[idx] = temp[idx];
    }
}

static void mergeSortHelper(std::vector<int>& a, std::vector<int>& temp,
                            int left, int right, uint64_t& comps) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortHelper(a, temp, left, mid, comps);
        mergeSortHelper(a, temp, mid + 1, right, comps);
        merge(a, temp, left, mid, right, comps);
    }
}

void mergeSort(std::vector<int>& a, uint64_t& comps) {
    int n = (int)a.size();
    if (n <= 1) return;
    std::vector<int> temp(n);
    mergeSortHelper(a, temp, 0, n - 1, comps);
}

/* --- QuickSort ---
 *
 * PARTITION SCHEME: Lomuto partition
 * ----------------------------------
 * The pivot is placed at a[high] (swapped there first if needed).
 * We maintain an index i such that all elements a[low..i] <= pivot.
 * We scan j from low to high-1; whenever a[j] <= pivot, we increment
 * i and swap a[i] with a[j].  Finally the pivot is placed at a[i+1].
 *
 * Each comparison a[j] <= pivot is a key comparison and is counted.
 *
 * STACK SAFETY
 * ------------
 * After partitioning, we recurse on the smaller sub-array first and
 * loop (tail-call optimise) on the larger one.  This guarantees
 * O(log n) stack depth even on worst-case inputs.
 */

// Lomuto partition with pivot already at a[high].
static int lomutoPartition(std::vector<int>& a, int low, int high, uint64_t& comps) {
    int pivot = a[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        ++comps;  // key comparison: a[j] <= pivot
        if (a[j] <= pivot) {
            ++i;
            std::swap(a[i], a[j]);
        }
    }
    std::swap(a[i + 1], a[high]);
    return i + 1;
}

// QuickSort with tail-call optimisation on the larger partition.
static void quickSortInternal(std::vector<int>& a, int low, int high,
                              uint64_t& comps, int pivotStrategy) {
    while (low < high) {
        // Select pivot and move it to a[high]
        int pivotIdx;
        if (pivotStrategy == 0) {
            // First-element pivot
            pivotIdx = low;
        } else if (pivotStrategy == 1) {
            // Random-element pivot
            std::uniform_int_distribution<int> dist(low, high);
            pivotIdx = dist(pivotRng);
        } else {
            // Median-of-three pivot
            int mid = low + (high - low) / 2;
            // Find median of a[low], a[mid], a[high]
            // We count these as key comparisons since they compare data elements.
            int lo = low, mi = mid, hi2 = high;

            // Sort the three values to find the median
            ++comps; // key comparison
            if (a[lo] > a[mi]) std::swap(lo, mi);
            ++comps; // key comparison
            if (a[lo] > a[hi2]) std::swap(lo, hi2);
            ++comps; // key comparison
            if (a[mi] > a[hi2]) std::swap(mi, hi2);

            pivotIdx = mi;  // median index
        }

        std::swap(a[pivotIdx], a[high]);

        int p = lomutoPartition(a, low, high, comps);

        // Recurse on smaller partition, loop on larger (tail-call opt)
        if (p - low < high - p) {
            quickSortInternal(a, low, p - 1, comps, pivotStrategy);
            low = p + 1;
        } else {
            quickSortInternal(a, p + 1, high, comps, pivotStrategy);
            high = p - 1;
        }
    }
}

void quickSortFirst(std::vector<int>& a, uint64_t& comps) {
    if (a.size() <= 1) return;
    quickSortInternal(a, 0, (int)a.size() - 1, comps, 0);
}

void quickSortRandom(std::vector<int>& a, uint64_t& comps) {
    if (a.size() <= 1) return;
    // Re-seed the pivot RNG for reproducibility of this sort call.
    pivotRng.seed(PIVOT_SEED);
    quickSortInternal(a, 0, (int)a.size() - 1, comps, 1);
}

void quickSortMedian3(std::vector<int>& a, uint64_t& comps) {
    if (a.size() <= 1) return;
    quickSortInternal(a, 0, (int)a.size() - 1, comps, 2);
}

/* --- Heap Sort ---
 *
 * Standard in-place heap sort using a max-heap.
 * siftDown restores the heap property; each comparison of
 * a[child] vs a[child+1] and a[largest] vs a[i] is a key comparison.
 */
static void siftDown(std::vector<int>& a, int n, int i, uint64_t& comps) {
    while (true) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n) {
            ++comps;  // key comparison: a[left] > a[largest]
            if (a[left] > a[largest]) {
                largest = left;
            }
        }
        if (right < n) {
            ++comps;  // key comparison: a[right] > a[largest]
            if (a[right] > a[largest]) {
                largest = right;
            }
        }

        if (largest == i) break;  // NOT a key comparison (index comparison)

        std::swap(a[i], a[largest]);
        i = largest;
    }
}

void heapSort(std::vector<int>& a, uint64_t& comps) {
    int n = (int)a.size();
    // Build max-heap
    for (int i = n / 2 - 1; i >= 0; --i) {
        siftDown(a, n, i, comps);
    }
    // Extract elements
    for (int i = n - 1; i > 0; --i) {
        std::swap(a[0], a[i]);
        siftDown(a, i, 0, comps);
    }
}

/* --- Radix Sort ---
 *
 * LSD Radix Sort for non-negative integers, base 10.
 *
 * This implementation supports non-negative integers only.
 * The input generation guarantees non-negative values
 * (random range [0, n*10], increasing [0..n-1], decreasing [n-1..0]).
 *
 * Because Radix Sort determines ordering by examining individual
 * digits rather than comparing keys, the key-comparison count
 * is always 0.
 */
void radixSort(std::vector<int>& a, uint64_t& comps) {
    // comps stays 0 — Radix Sort is non-comparison-based.
    (void)comps;

    int n = (int)a.size();
    if (n <= 1) return;

    // Find maximum value to determine number of digit passes
    int maxVal = *std::max_element(a.begin(), a.end());

    std::vector<int> output(n);

    // Process each digit (LSD first)
    for (long long exp = 1; maxVal / exp > 0; exp *= 10) {
        int count[10] = {0};

        // Count occurrences
        for (int i = 0; i < n; ++i) {
            count[(a[i] / (int)exp) % 10]++;
        }

        // Cumulative count
        for (int i = 1; i < 10; ++i) {
            count[i] += count[i - 1];
        }

        // Build output (stable, right to left)
        for (int i = n - 1; i >= 0; --i) {
            int digit = (a[i] / (int)exp) % 10;
            output[count[digit] - 1] = a[i];
            count[digit]--;
        }

        // Copy back
        for (int i = 0; i < n; ++i) {
            a[i] = output[i];
        }
    }
}

/* ------------------------------------------------------------------ */
/*                    SORTING DISPATCH                                  */
/* ------------------------------------------------------------------ */

// Function pointer type for sorting algorithms
typedef void (*SortFunc)(std::vector<int>&, uint64_t&);

SortFunc getSortFunction(int algoIdx) {
    switch (algoIdx) {
        case 0: return bubbleSort;
        case 1: return insertionSort;
        case 2: return mergeSort;
        case 3: return quickSortFirst;
        case 4: return quickSortRandom;
        case 5: return quickSortMedian3;
        case 6: return heapSort;
        case 7: return radixSort;
        default: return NULL;
    }
}

/* ------------------------------------------------------------------ */
/*                EXPERIMENT HARNESS                                    */
/* ------------------------------------------------------------------ */

struct TrialResult {
    std::string algorithm;
    std::string inputType;
    int n;
    int trial;
    int64_t time_us;
    uint64_t comparisons;
};

// Run a single timed trial of one algorithm on a copy of the input data.
// Timing excludes copying, verification, and everything else.
TrialResult runTrial(int algoIdx, const std::vector<int>& inputData,
                     const std::string& inputType, int trialNum) {
    TrialResult result;
    result.algorithm = ALGO_NAMES[algoIdx];
    result.inputType = inputType;
    result.n = (int)inputData.size();
    result.trial = trialNum;

    // Copy the input so the original is unchanged
    std::vector<int> data(inputData);

    uint64_t comps = 0;
    SortFunc sortFn = getSortFunction(algoIdx);

    // --- TIMED REGION START ---
    auto t1 = std::chrono::high_resolution_clock::now();
    sortFn(data, comps);
    auto t2 = std::chrono::high_resolution_clock::now();
    // --- TIMED REGION END ---

    result.time_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    result.comparisons = comps;

    // Correctness verification (outside timed region)
    if (!std::is_sorted(data.begin(), data.end())) {
        std::cerr << "ERROR: " << result.algorithm << " produced incorrect result!"
                  << " (input_type=" << inputType
                  << ", n=" << result.n
                  << ", trial=" << trialNum << ")" << std::endl;
        exit(1);
    }

    return result;
}

// Write results to CSV
void writeCSV(const std::vector<TrialResult>& results, const std::string& filename) {
    std::ofstream fout(filename);
    if (!fout.is_open()) {
        std::cerr << "Error: Cannot open " << filename << " for writing." << std::endl;
        exit(1);
    }

    fout << "algorithm,input_type,n,trial,time_us,comparisons" << std::endl;
    for (size_t i = 0; i < results.size(); ++i) {
        const TrialResult& r = results[i];
        fout << r.algorithm << "," << r.inputType << ","
             << r.n << "," << r.trial << ","
             << r.time_us << "," << r.comparisons << std::endl;
    }

    fout.close();
}

// Append results to an existing CSV (for incremental writing)
void appendCSV(const std::vector<TrialResult>& results, const std::string& filename) {
    // Check if file exists and has content
    std::ifstream check(filename);
    bool exists = check.good();
    check.close();

    std::ofstream fout(filename, std::ios::app);
    if (!fout.is_open()) {
        std::cerr << "Error: Cannot open " << filename << " for appending." << std::endl;
        exit(1);
    }

    if (!exists) {
        fout << "algorithm,input_type,n,trial,time_us,comparisons" << std::endl;
    }

    for (size_t i = 0; i < results.size(); ++i) {
        const TrialResult& r = results[i];
        fout << r.algorithm << "," << r.inputType << ","
             << r.n << "," << r.trial << ","
             << r.time_us << "," << r.comparisons << std::endl;
    }

    fout.close();
}

// Print summary for a set of trials of the same algorithm/input/size
void printSummary(const std::vector<TrialResult>& trials) {
    if (trials.empty()) return;

    const TrialResult& first = trials[0];

    int64_t minTime = trials[0].time_us;
    int64_t sumTime = 0;
    uint64_t minComps = trials[0].comparisons;
    uint64_t sumComps = 0;

    for (size_t i = 0; i < trials.size(); ++i) {
        if (trials[i].time_us < minTime) minTime = trials[i].time_us;
        sumTime += trials[i].time_us;
        if (trials[i].comparisons < minComps) minComps = trials[i].comparisons;
        sumComps += trials[i].comparisons;
    }

    double avgTime = (double)sumTime / trials.size();
    double avgComps = (double)sumComps / trials.size();

    std::cout << "  Summary: " << first.algorithm
              << " | " << first.inputType
              << " | n=" << first.n
              << " | min_time=" << minTime << "us"
              << " | avg_time=" << std::fixed << std::setprecision(1) << avgTime << "us"
              << " | min_comps=" << minComps
              << " | avg_comps=" << std::fixed << std::setprecision(0) << avgComps
              << std::endl;
}

// Run all trials for a single algorithm/input/size combination
std::vector<TrialResult> runExperiment(int algoIdx, const std::string& inputType,
                                       int n, int numTrials) {
    std::vector<TrialResult> trials;

    for (int t = 1; t <= numTrials; ++t) {
        std::cout << "Running " << ALGO_NAMES[algoIdx]
                  << " | " << inputType
                  << " | n=" << n
                  << " | trial=" << t << std::endl;

        // Generate input (outside timed region)
        std::vector<int> input = generateInput(inputType, n, t);

        TrialResult result = runTrial(algoIdx, input, inputType, t);
        trials.push_back(result);

        std::cout << "  -> time=" << result.time_us << "us"
                  << "  (" << std::fixed << std::setprecision(6)
                  << (result.time_us / 1000000.0) << "s)"
                  << "  comparisons=" << result.comparisons << std::endl;
    }

    printSummary(trials);
    std::cout << std::endl;

    return trials;
}

// Run all trials for a single algorithm on file input
std::vector<TrialResult> runExperimentFile(int algoIdx, const std::vector<int>& input,
                                            int numTrials) {
    std::vector<TrialResult> trials;
    int n = (int)input.size();

    for (int t = 1; t <= numTrials; ++t) {
        std::cout << "Running " << ALGO_NAMES[algoIdx]
                  << " | file"
                  << " | n=" << n
                  << " | trial=" << t << std::endl;

        TrialResult result = runTrial(algoIdx, input, "file", t);
        trials.push_back(result);

        std::cout << "  -> time=" << result.time_us << "us"
                  << "  (" << std::fixed << std::setprecision(6)
                  << (result.time_us / 1000000.0) << "s)"
                  << "  comparisons=" << result.comparisons << std::endl;
    }

    printSummary(trials);
    std::cout << std::endl;

    return trials;
}

/* ------------------------------------------------------------------ */
/*                    FULL EXPERIMENT SUITE                             */
/* ------------------------------------------------------------------ */

void runFullSuite() {
    std::vector<TrialResult> allResults;

    std::cout << "========================================" << std::endl;
    std::cout << "  FULL EXPERIMENT SUITE" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Sizes: ";
    for (int s = 0; s < NUM_SIZES; ++s) {
        std::cout << SIZES[s];
        if (s < NUM_SIZES - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    std::cout << "Input types: random, increasing, decreasing" << std::endl;
    std::cout << "Trials per combination: " << NUM_TRIALS << std::endl;
    std::cout << "Algorithms: " << NUM_ALGOS << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    for (int si = 0; si < NUM_SIZES; ++si) {
        int n = SIZES[si];
        for (int it = 0; it < NUM_INPUT_TYPES; ++it) {
            std::string inputType = INPUT_TYPES[it];

            // Generate all trial inputs upfront so all algorithms
            // use identical data for the same (input_type, n, trial).
            std::vector<std::vector<int> > trialInputs(NUM_TRIALS);
            for (int t = 0; t < NUM_TRIALS; ++t) {
                trialInputs[t] = generateInput(inputType, n, t + 1);
            }

            for (int ai = 0; ai < NUM_ALGOS; ++ai) {
                // Skip Bubble Sort for large n
                if (ai == 0 && n > BUBBLE_SORT_MAX_N) {
                    std::cout << "Skipping " << ALGO_NAMES[ai]
                              << " for n=" << n << " (n > "
                              << BUBBLE_SORT_MAX_N << ")" << std::endl << std::endl;
                    continue;
                }

                std::vector<TrialResult> trials;
                for (int t = 0; t < NUM_TRIALS; ++t) {
                    std::cout << "Running " << ALGO_NAMES[ai]
                              << " | " << inputType
                              << " | n=" << n
                              << " | trial=" << (t + 1) << std::endl;

                    TrialResult result = runTrial(ai, trialInputs[t], inputType, t + 1);
                    trials.push_back(result);

                    std::cout << "  -> time=" << result.time_us << "us"
                              << "  (" << std::fixed << std::setprecision(6)
                              << (result.time_us / 1000000.0) << "s)"
                              << "  comparisons=" << result.comparisons << std::endl;
                }

                printSummary(trials);
                std::cout << std::endl;

                allResults.insert(allResults.end(), trials.begin(), trials.end());
            }
        }
    }

    writeCSV(allResults, "results.csv");
    std::cout << "========================================" << std::endl;
    std::cout << "Results written to results.csv" << std::endl;
    std::cout << "Total records: " << allResults.size() << std::endl;
    std::cout << "========================================" << std::endl;
}

/* ------------------------------------------------------------------ */
/*                    INTERACTIVE MENU                                  */
/* ------------------------------------------------------------------ */

int selectAlgorithm() {
    std::cout << std::endl;
    std::cout << "Select algorithm:" << std::endl;
    for (int i = 0; i < NUM_ALGOS; ++i) {
        std::cout << "  " << (i + 1) << ". " << ALGO_NAMES[i] << std::endl;
    }
    std::cout << "Choice: ";
    int choice;
    std::cin >> choice;
    if (choice < 1 || choice > NUM_ALGOS) {
        std::cout << "Invalid choice." << std::endl;
        return -1;
    }
    return choice - 1;
}

std::string selectInputType() {
    std::cout << std::endl;
    std::cout << "Select input type:" << std::endl;
    std::cout << "  1. Random" << std::endl;
    std::cout << "  2. Increasing" << std::endl;
    std::cout << "  3. Decreasing" << std::endl;
    std::cout << "Choice: ";
    int choice;
    std::cin >> choice;
    switch (choice) {
        case 1: return "random";
        case 2: return "increasing";
        case 3: return "decreasing";
        default:
            std::cout << "Invalid choice." << std::endl;
            return "";
    }
}

int selectSize() {
    std::cout << std::endl;
    std::cout << "Enter input size n: ";
    int n;
    std::cin >> n;
    if (n <= 0) {
        std::cout << "Invalid size." << std::endl;
        return -1;
    }
    return n;
}

void menuSingleExperiment() {
    int algoIdx = selectAlgorithm();
    if (algoIdx < 0) return;

    std::cout << std::endl;
    std::cout << "Input source:" << std::endl;
    std::cout << "  1. Generated" << std::endl;
    std::cout << "  2. File" << std::endl;
    std::cout << "Choice: ";
    int srcChoice;
    std::cin >> srcChoice;

    std::vector<TrialResult> results;

    if (srcChoice == 1) {
        std::string inputType = selectInputType();
        if (inputType.empty()) return;

        int n = selectSize();
        if (n < 0) return;

        // Check Bubble Sort restriction
        if (algoIdx == 0 && n > BUBBLE_SORT_MAX_N) {
            std::cout << "WARNING: Bubble Sort is restricted to n <= "
                      << BUBBLE_SORT_MAX_N << ". Aborting." << std::endl;
            return;
        }

        results = runExperiment(algoIdx, inputType, n, NUM_TRIALS);
    } else if (srcChoice == 2) {
        std::cout << "Enter filename: ";
        std::string filename;
        std::cin >> filename;

        std::vector<int> input = readInputFromFile(filename);
        int n = (int)input.size();

        // Check Bubble Sort restriction
        if (algoIdx == 0 && n > BUBBLE_SORT_MAX_N) {
            std::cout << "WARNING: Bubble Sort is restricted to n <= "
                      << BUBBLE_SORT_MAX_N << ". Aborting." << std::endl;
            return;
        }

        results = runExperimentFile(algoIdx, input, NUM_TRIALS);
    } else {
        std::cout << "Invalid choice." << std::endl;
        return;
    }

    // Append to CSV
    appendCSV(results, "results.csv");
    std::cout << "Results appended to results.csv" << std::endl;
}

void menuAllAlgorithms() {
    std::string inputType = selectInputType();
    if (inputType.empty()) return;

    int n = selectSize();
    if (n < 0) return;

    std::vector<TrialResult> allResults;

    // Generate trial inputs upfront
    std::vector<std::vector<int> > trialInputs(NUM_TRIALS);
    for (int t = 0; t < NUM_TRIALS; ++t) {
        trialInputs[t] = generateInput(inputType, n, t + 1);
    }

    for (int ai = 0; ai < NUM_ALGOS; ++ai) {
        if (ai == 0 && n > BUBBLE_SORT_MAX_N) {
            std::cout << "Skipping " << ALGO_NAMES[ai]
                      << " for n=" << n << " (n > "
                      << BUBBLE_SORT_MAX_N << ")" << std::endl << std::endl;
            continue;
        }

        std::vector<TrialResult> trials;
        for (int t = 0; t < NUM_TRIALS; ++t) {
            std::cout << "Running " << ALGO_NAMES[ai]
                      << " | " << inputType
                      << " | n=" << n
                      << " | trial=" << (t + 1) << std::endl;

            TrialResult result = runTrial(ai, trialInputs[t], inputType, t + 1);
            trials.push_back(result);

            std::cout << "  -> time=" << result.time_us << "us"
                      << "  comparisons=" << result.comparisons << std::endl;
        }

        printSummary(trials);
        std::cout << std::endl;

        allResults.insert(allResults.end(), trials.begin(), trials.end());
    }

    appendCSV(allResults, "results.csv");
    std::cout << "Results appended to results.csv" << std::endl;
}

void menuFileInput() {
    std::cout << "Enter filename: ";
    std::string filename;
    std::cin >> filename;

    std::vector<int> input = readInputFromFile(filename);
    std::cout << "Loaded " << input.size() << " integers from " << filename << std::endl;

    int algoIdx = selectAlgorithm();
    if (algoIdx < 0) return;

    int n = (int)input.size();
    if (algoIdx == 0 && n > BUBBLE_SORT_MAX_N) {
        std::cout << "WARNING: Bubble Sort is restricted to n <= "
                  << BUBBLE_SORT_MAX_N << ". Aborting." << std::endl;
        return;
    }

    std::vector<TrialResult> results = runExperimentFile(algoIdx, input, NUM_TRIALS);

    appendCSV(results, "results.csv");
    std::cout << "Results appended to results.csv" << std::endl;
}

void interactiveMenu() {
    while (true) {
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "  SORTING ALGORITHM COMPARISON" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "  1. Run a single algorithm experiment" << std::endl;
        std::cout << "  2. Run all algorithms for one input type and size" << std::endl;
        std::cout << "  3. Run the complete experiment suite" << std::endl;
        std::cout << "  4. Load an input from a file" << std::endl;
        std::cout << "  5. Exit" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Exiting." << std::endl;
            break;
        }

        switch (choice) {
            case 1:
                menuSingleExperiment();
                break;
            case 2:
                menuAllAlgorithms();
                break;
            case 3:
                runFullSuite();
                break;
            case 4:
                menuFileInput();
                break;
            case 5:
                std::cout << "Exiting." << std::endl;
                return;
            default:
                std::cout << "Invalid choice. Try again." << std::endl;
        }
    }
}

/* ------------------------------------------------------------------ */
/*                           MAIN                                      */
/* ------------------------------------------------------------------ */

int main() {
    std::cout << "Sorting Algorithm Experimental Comparison" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Input seed: " << INPUT_SEED << std::endl;
    std::cout << "Pivot seed: " << PIVOT_SEED << std::endl;
    std::cout << "Trials per experiment: " << NUM_TRIALS << std::endl;
    std::cout << "Bubble Sort max n: " << BUBBLE_SORT_MAX_N << std::endl;
    std::cout << std::endl;

    interactiveMenu();

    return 0;
}
