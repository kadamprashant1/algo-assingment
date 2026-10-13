# CS-700 Assignment 2: Comparison of Sorting Algorithms

## Requirements
- g++ with C++17 support
- Python 3
- `pip install pandas matplotlib`
- A LaTeX install (only needed for the report)

## Run
Run everything (inputs, compile, benchmarks, graphs):

    python run_all.py

Only redraw the graphs from the existing CSV files:

    python run_all.py plots

Run from the project folder. The full benchmark takes a while because bubble, insertion and the first-pivot quick sort are slow on 100000 elements.

## Folders and files
- `src/` the 8 C++ programs, each one standalone
- `bin/` compiled programs (created by `run_all.py`)
- `input_random.txt`, `input_increasing.txt`, `input_decreasing.txt` input files (created by `run_all.py`)
- `results/` one CSV per algorithm plus `summary.csv`
- `graphs/` all PNG graphs
- `run_all.py` the only Python file
