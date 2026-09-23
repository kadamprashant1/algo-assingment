#!/usr/bin/env python3
"""
plot_results.py
===============
Reads results.csv produced by sort_compare.cpp and generates all
required analysis plots and a summary CSV.

Dependencies: pandas, matplotlib, numpy, scipy

Output files (saved to plots/ directory):
  - quicksort_increasing.png
  - quicksort_decreasing.png
  - quicksort_random.png
  - allsorts_increasing.png
  - allsorts_decreasing.png
  - allsorts_random.png
  - comparisons_vs_n.png
  - time_per_comparison.png
  - time_vs_comparisons.png
  - summary.csv

WHY MINIMUM RUNTIME IS THE PRIMARY METRIC
------------------------------------------
The minimum across 5 trials reduces the influence of OS scheduling
and background noise.  It represents the closest measurement to the
algorithm's true cost on the given hardware.  "Minimum measured
runtime" is NOT the same as "theoretical best-case complexity".

INCREASING / DECREASING / RANDOM INPUTS
-----------------------------------------
These are fixed input distributions used for experimentation.
They are NOT automatically equivalent to theoretical best/worst/
average cases for every algorithm.

RADIX SORT AND COMPARISONS
----------------------------
Radix Sort is non-comparison-based; its key-comparison count is 0.
It is excluded from log-scale comparison plots and from the
time-per-comparison metric because log(0) is undefined and
division by zero is invalid.
"""

import os
import sys
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend; works without GUI
import matplotlib.pyplot as plt

try:
    from scipy import stats as scipy_stats
    HAS_SCIPY = True
except ImportError:
    HAS_SCIPY = False
    print("Note: scipy not available. Regression will use numpy.")

# ------------------------------------------------------------------ #
#                     CONFIGURATION                                    #
# ------------------------------------------------------------------ #

CSV_FILE = "results.csv"
OUTPUT_DIR = "plots"
SUMMARY_FILE = "summary.csv"

REQUIRED_COLUMNS = ["algorithm", "input_type", "n", "trial",
                    "time_us", "comparisons"]

# Consistent colour map for algorithms across all plots.
ALGO_COLORS = {
    "BubbleSort":       "#e74c3c",   # red
    "InsertionSort":    "#e67e22",   # orange
    "MergeSort":        "#2ecc71",   # green
    "QuickSort_First":  "#3498db",   # blue
    "QuickSort_Random": "#9b59b6",   # purple
    "QuickSort_Median3":"#1abc9c",   # teal
    "HeapSort":         "#f39c12",   # amber
    "RadixSort":        "#34495e",   # dark grey
}

ALGO_MARKERS = {
    "BubbleSort":       "o",
    "InsertionSort":    "s",
    "MergeSort":        "^",
    "QuickSort_First":  "D",
    "QuickSort_Random": "v",
    "QuickSort_Median3":"P",
    "HeapSort":         "X",
    "RadixSort":        "*",
}

# Map from CSV input_type to human-readable label
INPUT_LABELS = {
    "random":     "Random Input",
    "increasing": "Increasing (Ordered) Input",
    "decreasing": "Decreasing (Reverse-Ordered) Input",
}

# For the six-sort comparison, use QuickSort_Median3 as the QuickSort
# representative (stated clearly in titles and comments).
QUICKSORT_REPRESENTATIVE = "QuickSort_Median3"

SIX_SORT_ALGOS = [
    "BubbleSort",
    "InsertionSort",
    "MergeSort",
    QUICKSORT_REPRESENTATIVE,  # represents "QuickSort"
    "HeapSort",
    "RadixSort",
]

# Display name for the six-sort comparison legend
SIX_SORT_LABELS = {
    "BubbleSort":        "Bubble Sort",
    "InsertionSort":     "Insertion Sort",
    "MergeSort":         "Merge Sort",
    QUICKSORT_REPRESENTATIVE: "Quick Sort (Median-of-3)",
    "HeapSort":          "Heap Sort",
    "RadixSort":         "Radix Sort",
}


# ------------------------------------------------------------------ #
#                     HELPER FUNCTIONS                                 #
# ------------------------------------------------------------------ #

def ensure_output_dir():
    """Create the output directory if it does not exist."""
    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)


def load_data():
    """Load and validate results.csv."""
    if not os.path.exists(CSV_FILE):
        print(f"Error: '{CSV_FILE}' not found. Run sort_compare first.")
        sys.exit(1)

    df = pd.read_csv(CSV_FILE)

    # Validate columns
    missing = [c for c in REQUIRED_COLUMNS if c not in df.columns]
    if missing:
        print(f"Error: Missing columns in CSV: {missing}")
        sys.exit(1)

    return df


def aggregate(df):
    """
    Aggregate trials: compute min and average for time and comparisons.
    Returns a DataFrame with one row per (algorithm, input_type, n).
    """
    agg = df.groupby(["algorithm", "input_type", "n"]).agg(
        min_time=("time_us", "min"),
        avg_time=("time_us", "mean"),
        min_comparisons=("comparisons", "min"),
        avg_comparisons=("comparisons", "mean"),
        num_trials=("trial", "count"),
    ).reset_index()

    return agg


def save_summary(agg):
    """Save the aggregated summary to summary.csv."""
    agg.to_csv(SUMMARY_FILE, index=False)
    print(f"Summary saved to {SUMMARY_FILE}")


def get_color(algo):
    return ALGO_COLORS.get(algo, "#7f8c8d")


def get_marker(algo):
    return ALGO_MARKERS.get(algo, "o")


def savefig(fig, filename):
    """Save figure to the output directory."""
    path = os.path.join(OUTPUT_DIR, filename)
    fig.savefig(path, dpi=300, bbox_inches="tight")
    print(f"  Saved: {path}")


# ------------------------------------------------------------------ #
#            REUSABLE PLOTTING FUNCTION                                #
# ------------------------------------------------------------------ #

def plot_algorithms(agg, algorithms, input_type, title, filename,
                    y_column="min_time", y_label="Minimum Runtime (µs)",
                    loglog=True, labels=None):
    """
    Generic line plot: x = n, y = y_column, one line per algorithm.
    
    Parameters
    ----------
    agg        : aggregated DataFrame
    algorithms : list of algorithm names to include
    input_type : which input_type to filter on
    title      : plot title
    filename   : output PNG filename
    y_column   : column to plot on y-axis
    y_label    : y-axis label
    loglog     : whether to use log-log axes
    labels     : optional dict mapping algorithm -> display label
    """
    fig, ax = plt.subplots(figsize=(10, 6))

    subset = agg[agg["input_type"] == input_type]

    for algo in algorithms:
        data = subset[subset["algorithm"] == algo].sort_values("n")
        if data.empty:
            continue

        label = labels[algo] if labels and algo in labels else algo

        ax.plot(data["n"], data[y_column],
                color=get_color(algo),
                marker=get_marker(algo),
                markersize=7,
                linewidth=2,
                label=label)

    if loglog:
        ax.set_xscale("log")
        ax.set_yscale("log")

    ax.set_xlabel("Input Size (n)", fontsize=12)
    ax.set_ylabel(y_label, fontsize=12)
    ax.set_title(title, fontsize=14, fontweight="bold")
    ax.legend(fontsize=10)
    ax.grid(True, which="both", linestyle="--", alpha=0.5)
    fig.tight_layout()

    savefig(fig, filename)
    plt.close(fig)


# ------------------------------------------------------------------ #
#                    PLOT GENERATORS                                   #
# ------------------------------------------------------------------ #

def plot_quicksort_comparisons(agg):
    """
    Generate three QuickSort pivot-comparison plots:
      quicksort_increasing.png
      quicksort_decreasing.png
      quicksort_random.png
    """
    qs_algos = ["QuickSort_First", "QuickSort_Random", "QuickSort_Median3"]

    for input_type in ["increasing", "decreasing", "random"]:
        label = INPUT_LABELS.get(input_type, input_type)
        title = f"QuickSort Pivot Comparison — {label}"
        filename = f"quicksort_{input_type}.png"

        plot_algorithms(agg, qs_algos, input_type, title, filename,
                        y_column="min_time",
                        y_label="Minimum Runtime (µs)",
                        loglog=True)


def plot_six_sort_comparison(agg):
    """
    Generate three six-sort comparison plots using QuickSort_Median3
    as the QuickSort representative:
      allsorts_increasing.png
      allsorts_decreasing.png
      allsorts_random.png

    Bubble Sort may be missing for large n; only available points
    are plotted (no interpolation).
    """
    for input_type in ["increasing", "decreasing", "random"]:
        label = INPUT_LABELS.get(input_type, input_type)
        title = (f"Six-Sort Comparison — {label}\n"
                 f"(QuickSort uses Median-of-3 pivot)")
        filename = f"allsorts_{input_type}.png"

        plot_algorithms(agg, SIX_SORT_ALGOS, input_type, title, filename,
                        y_column="min_time",
                        y_label="Minimum Runtime (µs)",
                        loglog=True,
                        labels=SIX_SORT_LABELS)


def plot_comparisons_vs_n(agg):
    """
    Plot number of key comparisons vs n for every algorithm.

    Radix Sort has comparisons = 0 and is excluded from the log-log plot
    because log(0) is undefined.  A note is printed instead.
    """
    fig, ax = plt.subplots(figsize=(10, 7))

    # Use random input for this composite plot
    subset = agg[agg["input_type"] == "random"]

    algos = sorted(subset["algorithm"].unique())

    for algo in algos:
        data = subset[subset["algorithm"] == algo].sort_values("n")
        if data.empty:
            continue

        # Skip Radix Sort on log scale (comparisons == 0)
        if algo == "RadixSort":
            continue

        ax.plot(data["n"], data["min_comparisons"],
                color=get_color(algo),
                marker=get_marker(algo),
                markersize=7,
                linewidth=2,
                label=algo)

    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlabel("Input Size (n)", fontsize=12)
    ax.set_ylabel("Minimum Key Comparisons", fontsize=12)
    ax.set_title("Key Comparisons vs Input Size — Random Input\n"
                 "(Radix Sort excluded: 0 comparisons, non-comparison-based)",
                 fontsize=13, fontweight="bold")
    ax.legend(fontsize=9)
    ax.grid(True, which="both", linestyle="--", alpha=0.5)
    fig.tight_layout()

    savefig(fig, "comparisons_vs_n.png")
    plt.close(fig)


def plot_time_per_comparison(agg):
    """
    Plot time_per_comparison = time_us / comparisons vs n.
    Only for comparison-based algorithms (comparisons > 0).
    Radix Sort is excluded.
    """
    fig, ax = plt.subplots(figsize=(10, 7))

    subset = agg[(agg["input_type"] == "random") &
                 (agg["min_comparisons"] > 0)].copy()

    subset["time_per_comp"] = subset["min_time"] / subset["min_comparisons"]

    algos = sorted(subset["algorithm"].unique())

    for algo in algos:
        data = subset[subset["algorithm"] == algo].sort_values("n")
        if data.empty:
            continue

        ax.plot(data["n"], data["time_per_comp"],
                color=get_color(algo),
                marker=get_marker(algo),
                markersize=7,
                linewidth=2,
                label=algo)

    ax.set_xlabel("Input Size (n)", fontsize=12)
    ax.set_ylabel("Microseconds per Key Comparison", fontsize=12)
    ax.set_title("Time per Key Comparison vs Input Size — Random Input\n"
                 "(Radix Sort excluded: non-comparison-based)",
                 fontsize=13, fontweight="bold")
    ax.legend(fontsize=9)
    ax.grid(True, which="both", linestyle="--", alpha=0.5)
    fig.tight_layout()

    savefig(fig, "time_per_comparison.png")
    plt.close(fig)


def plot_time_vs_comparisons(agg):
    """
    Scatter plot: x = comparisons, y = runtime.
    Uses all valid comparison-based observations (all input types).
    Fits a regression line and displays Pearson's r.
    Radix Sort excluded (comparisons = 0).

    Note: correlation does not imply causation.
    """
    fig, ax = plt.subplots(figsize=(10, 7))

    # All input types, comparison-based only
    valid = agg[agg["min_comparisons"] > 0].copy()

    if valid.empty:
        print("  Warning: No comparison-based data for time_vs_comparisons plot.")
        plt.close(fig)
        return

    algos = sorted(valid["algorithm"].unique())

    # Scatter by algorithm for colour coding
    all_x = []
    all_y = []
    for algo in algos:
        data = valid[valid["algorithm"] == algo]
        x = data["min_comparisons"].values
        y = data["min_time"].values
        all_x.extend(x)
        all_y.extend(y)

        ax.scatter(x, y,
                   color=get_color(algo),
                   marker=get_marker(algo),
                   s=60,
                   label=algo,
                   alpha=0.8,
                   edgecolors="white",
                   linewidths=0.5)

    all_x = np.array(all_x, dtype=float)
    all_y = np.array(all_y, dtype=float)

    # Regression and correlation
    if len(all_x) >= 2:
        if HAS_SCIPY:
            slope, intercept, r_value, p_value, std_err = scipy_stats.linregress(
                all_x, all_y)
        else:
            coeffs = np.polyfit(all_x, all_y, 1)
            slope, intercept = coeffs[0], coeffs[1]
            # Manual Pearson's r
            r_value = np.corrcoef(all_x, all_y)[0, 1]

        # Plot regression line
        x_line = np.linspace(all_x.min(), all_x.max(), 200)
        y_line = slope * x_line + intercept
        ax.plot(x_line, y_line, "r--", linewidth=2,
                label=f"Regression (r = {r_value:.4f})")

        ax.text(0.05, 0.95,
                f"Pearson r = {r_value:.4f}\ny = {slope:.6f}x + {intercept:.2f}",
                transform=ax.transAxes,
                fontsize=10,
                verticalalignment="top",
                bbox=dict(boxstyle="round,pad=0.4", facecolor="wheat", alpha=0.7))

        print(f"  Pearson correlation (time vs comparisons): r = {r_value:.4f}")
    else:
        r_value = float("nan")
        print("  Warning: Not enough data points for regression.")

    ax.set_xlabel("Key Comparisons", fontsize=12)
    ax.set_ylabel("Runtime (µs)", fontsize=12)
    ax.set_title("Runtime vs Key Comparisons — All Input Types\n"
                 "(Radix Sort excluded; correlation ≠ causation)",
                 fontsize=13, fontweight="bold")
    ax.legend(fontsize=9, loc="lower right")
    ax.grid(True, which="both", linestyle="--", alpha=0.5)
    fig.tight_layout()

    savefig(fig, "time_vs_comparisons.png")
    plt.close(fig)

    return r_value


# ------------------------------------------------------------------ #
#                TEXTUAL SUMMARY                                       #
# ------------------------------------------------------------------ #

def print_textual_summary(df, agg, r_value=None):
    """Print a short summary of the dataset."""
    print()
    print("=" * 60)
    print("  DATASET SUMMARY")
    print("=" * 60)
    print(f"  Total rows loaded:    {len(df)}")
    print(f"  Algorithms found:     {sorted(df['algorithm'].unique())}")
    print(f"  Input types found:    {sorted(df['input_type'].unique())}")
    print(f"  Sizes found:          {sorted(df['n'].unique())}")

    # Check for missing combinations
    all_algos = sorted(df["algorithm"].unique())
    all_inputs = sorted(df["input_type"].unique())
    all_sizes = sorted(df["n"].unique())

    missing = []
    for algo in all_algos:
        for inp in all_inputs:
            for n in all_sizes:
                sub = df[(df["algorithm"] == algo) &
                         (df["input_type"] == inp) &
                         (df["n"] == n)]
                if sub.empty:
                    missing.append(f"    {algo} | {inp} | n={n}")

    if missing:
        print(f"  Missing combinations ({len(missing)}):")
        for m in missing:
            print(m)
    else:
        print("  Missing combinations: None")

    if r_value is not None and not np.isnan(r_value):
        print(f"  Pearson r (time vs comparisons): {r_value:.4f}")

    print()
    print("  NOTES:")
    print("  - 'Minimum runtime' is the primary metric (reduces OS noise)")
    print("  - Radix Sort comparisons = 0 (non-comparison-based algorithm)")
    print("  - Input distributions are NOT universally equivalent to")
    print("    theoretical best/worst/average cases for every algorithm")
    print("  - Measured runtime depends on implementation and hardware")
    print("=" * 60)
    print()


# ------------------------------------------------------------------ #
#                        MAIN                                          #
# ------------------------------------------------------------------ #

def main():
    print("=" * 60)
    print("  Sorting Algorithm Analysis — plot_results.py")
    print("=" * 60)
    print()

    # Load data
    df = load_data()
    print(f"Loaded {len(df)} rows from {CSV_FILE}")

    # Aggregate
    agg = aggregate(df)

    # Create output directory
    ensure_output_dir()

    # Save summary CSV
    save_summary(agg)

    # Generate plots
    print()
    print("Generating plots...")
    print()

    print("[1/7] QuickSort pivot comparison plots...")
    plot_quicksort_comparisons(agg)

    print("[2/7] Six-sort comparison plots...")
    plot_six_sort_comparison(agg)

    print("[3/7] Comparisons vs n plot...")
    plot_comparisons_vs_n(agg)

    print("[4/7] Time per comparison plot...")
    plot_time_per_comparison(agg)

    print("[5/7] Time vs comparisons scatter plot...")
    r_value = plot_time_vs_comparisons(agg)

    print()
    print("All plots generated successfully!")

    # Textual summary
    print_textual_summary(df, agg, r_value)

    # Also display interactively if possible
    try:
        plt.show()
    except Exception:
        pass  # Non-interactive environment


if __name__ == "__main__":
    main()
