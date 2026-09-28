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

#include <sys/stat.h>
#include <sys/types.h>

#include "bubble_sort.h"
#include "insertion_sort.h"
#include "merge_sort.h"
#include "quick_sort.h"
#include "heap_sort.h"
#include "radix_sort.h"

using namespace std; // correct accordingly

void saveSortedArray(const std::vector<int>& data, const std::string& algoName, const std::string& inputType, int n, int trialNum) {
    mkdir("outputs", 0777);
    std::string filename = "outputs/sorted_" + algoName + "_" + inputType + "_" + std::to_string(n) + "_trial" + std::to_string(trialNum) + ".txt";
    std::ofstream fout(filename);
    if(fout.is_open()) {
        for(int val : data) {
            fout << val << "\n";
        }
        fout.close();
    }
}
/* ------------------------------------------------------------------ */
/*                        CONFIGURATION                                */
/* ------------------------------------------------------------------ */

static const int INPUT_SEED = 42;       // Seed for input generation
static const int PIVOT_SEED = 12345;    // Seed for QuickSort_Random pivot
static const int NUM_TRIALS = 5;
static const int BUBBLE_SORT_MAX_N = 20000;

// Experiment sizes
static const int SIZES[] = {100, 500, 1000, 10000, 20000, 50000, 100000};
static const int NUM_SIZES = 7;

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

    saveSortedArray(data, result.algorithm, inputType, result.n, trialNum);

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
