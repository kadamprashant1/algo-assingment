import os
import sys
import random
import subprocess
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

algos = ["bubble", "insertion", "merge", "quick1", "quick2", "quick3", "heap", "radix"]
sizes = [100, 500, 1000, 5000, 10000, 20000, 50000, 100000]
kinds = ["increasing", "decreasing", "random"]
exe = ".exe" if os.name == "nt" else ""

title = {
    "bubble": "Bubble Sort",
    "insertion": "Insertion Sort",
    "merge": "Merge Sort",
    "heap": "Heap Sort",
    "radix": "Radix Sort",
    "quick1": "QuickSort (first element)",
    "quick2": "QuickSort (random element)",
    "quick3": "QuickSort (median-of-three)",
}
cases = {"best": "Best", "worst": "Worst", "avg": "Average"}
label = {"increasing": "Ascending input", "decreasing": "Descending input", "random": "Random input"}


def make_input():
    random.seed(1)
    a = [random.randint(1, 300000) for i in range(100000)]
    lists = {
        "input_random.txt": a,
        "input_increasing.txt": sorted(a),
        "input_decreasing.txt": sorted(a, reverse=True),
    }
    for name in lists:
        f = open(name, "w")
        f.write(str(len(a)) + "\n")
        for x in lists[name]:
            f.write(str(x) + "\n")
        f.close()
    print("inputs ready")


def compile_all():
    os.makedirs("bin", exist_ok=True)
    for x in algos:
        cmd = ["g++", "-O2", "-std=c++17", "-Wall", "src/" + x + ".cpp", "-o", "bin/" + x + exe]
        subprocess.run(cmd, check=True)
        print("compiled", x)


def run_all():
    os.makedirs("results", exist_ok=True)
    for x in algos:
        subprocess.run([os.path.join("bin", x + exe)], check=True)


def load():
    parts = []
    for x in algos:
        df = pd.read_csv("results/" + x + ".csv")
        df["algorithm"] = x
        parts.append(df)
    return pd.concat(parts, ignore_index=True)


def plot_lines(lines, name, head, ylabel, log=False):
    plt.figure(figsize=(8, 5))
    for lab, xs, ys in lines:
        plt.plot(xs, ys, marker="o", label=lab)
    if log:
        plt.xscale("log")
        plt.yscale("log")
    plt.title(head)
    plt.xlabel("Input size n (number of elements)")
    plt.ylabel(ylabel)
    plt.grid(True, which="both", alpha=0.4)
    plt.legend()
    plt.tight_layout()
    plt.savefig("graphs/" + name + ".png", dpi=150)
    plt.close()


def val(avg, x, k, n, col):
    r = avg[(avg["algorithm"] == x) & (avg["input_type"] == k) & (avg["n"] == n)]
    return r[col].iloc[0]


def series(avg, x, k, col, scale=1.0):
    return [val(avg, x, k, n, col) / scale for n in sizes]


def case(avg, x, which):
    ys = []
    for n in sizes:
        v = [val(avg, x, k, n, "avg_time_us") / 1000 for k in kinds]
        if which == "best":
            ys.append(min(v))
        elif which == "worst":
            ys.append(max(v))
        else:
            ys.append(val(avg, x, "random", n, "avg_time_us") / 1000)
    return ys


def make_graphs():
    os.makedirs("graphs", exist_ok=True)
    df = load()
    avg = df[df["run"] == "avg"].copy()
    avg["n"] = avg["n"].astype(int)
    avg["avg_time_us"] = avg["time_us"].astype(float)
    avg["avg_comparisons"] = avg["comparisons"].astype(float)
    avg["time_per_comparison_us"] = avg["avg_time_us"] / avg["avg_comparisons"]
    avg.loc[avg["algorithm"] == "radix", "time_per_comparison_us"] = float("nan")
    avg.loc[avg["avg_comparisons"] == 0, "time_per_comparison_us"] = float("nan")
    cols = ["algorithm", "input_type", "n", "avg_time_us", "avg_comparisons", "time_per_comparison_us"]
    avg[cols].to_csv("results/summary.csv", index=False)

    for x in ["bubble", "insertion", "merge", "heap", "quick1", "quick2", "quick3"]:
        log = x.startswith("quick")
        lt = []
        lc = []
        for k in kinds:
            lt.append((label[k], sizes, series(avg, x, k, "avg_time_us", 1000)))
            lc.append((label[k], sizes, series(avg, x, k, "avg_comparisons")))
        plot_lines(lt, x + "_time", title[x] + ": Execution Time vs Input Size", "Execution time (ms)", log)
        plot_lines(lc, x + "_cmp", title[x] + ": Comparisons vs Input Size", "Number of comparisons", log)

    qs = ["quick1", "quick2", "quick3"]
    for which in ["best", "worst", "avg"]:
        lines = [(title[x], sizes, case(avg, x, which)) for x in qs]
        plot_lines(lines, "quick_" + which, "QuickSort Pivot Strategies: " + cases[which] + " Case", "Execution time (ms)", True)

    lines = [(label[k], sizes, series(avg, "radix", k, "avg_time_us", 1000)) for k in kinds]
    plot_lines(lines, "radix_time", "Radix Sort: Execution Time vs Input Size", "Execution time (ms)")

    best = qs[0]
    for x in qs:
        if val(avg, x, "random", 100000, "avg_time_us") < val(avg, best, "random", 100000, "avg_time_us"):
            best = x
    print("best quick version:", best)

    six = ["bubble", "insertion", "merge", best, "heap", "radix"]
    names = {"bubble": "Bubble", "insertion": "Insertion", "merge": "Merge", "heap": "Heap", "radix": "Radix", best: title[best]}
    for which in ["best", "worst", "avg"]:
        lines = [(names[x], sizes, case(avg, x, which)) for x in six]
        plot_lines(lines, "all_" + which, "All Sorting Algorithms: " + cases[which] + " Case", "Execution time (ms)", True)

    five = ["bubble", "insertion", "merge", best, "heap"]
    lines = [(names[x], sizes, series(avg, x, "random", "time_per_comparison_us")) for x in five]
    plot_lines(lines, "cost_per_cmp", "Time per Comparison vs Input Size (random input)", "Time per comparison (microseconds)", True)
    print("graphs saved")


def main():
    if len(sys.argv) > 1 and sys.argv[1] == "plots":
        make_graphs()
        return
    make_input()
    compile_all()
    run_all()
    make_graphs()


if __name__ == "__main__":
    main()
