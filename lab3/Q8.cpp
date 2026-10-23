// Problem 8: Strassen's matrix multiplication vs the standard method
//
// Principle used: Divide and Conquer
//
// Idea:
//   Normal multiplication of two n x n matrices takes n^3 steps. If we split
//   each matrix into four n/2 x n/2 blocks, the obvious divide and conquer
//   needs 8 block multiplications -> T(n) = 8T(n/2) + O(n^2) = O(n^3), no gain.
//
//   Strassen found a way to do it with only 7 block multiplications (at the
//   cost of more additions and subtractions, which are cheap, O(n^2)):
//
//       M1 = (A11 + A22) * (B11 + B22)
//       M2 = (A21 + A22) * B11
//       M3 = A11 * (B12 - B22)
//       M4 = A22 * (B21 - B11)
//       M5 = (A11 + A12) * B22
//       M6 = (A21 - A11) * (B11 + B12)
//       M7 = (A12 - A22) * (B21 + B22)
//
//       C11 = M1 + M4 - M5 + M7
//       C12 = M3 + M5
//       C21 = M2 + M4
//       C22 = M1 - M2 + M3 + M6
//
//   So T(n) = 7T(n/2) + O(n^2) -> O(n^log2(7)) ~ O(n^2.81)
//
// Practical notes:
//   - Strassen only works directly when n is a power of 2, so we pad the
//     matrices with zeros up to the next power of 2 and cut the result back.
//   - For small blocks the overhead of all the additions and allocations is
//     more than the savings, so below CUTOFF we switch to the normal method.
//     This is what everybody does in practice.
//   - Don't be surprised if Strassen is NOT faster in the timing table for
//     these sizes. The standard loop is very cache friendly, while Strassen
//     allocates lots of temporary matrices. The asymptotic win (n^2.81 vs n^3)
//     only really shows up for much larger matrices, and even then a good
//     cutoff and less copying are needed.
//   - We use long long with small random values so the two methods can be
//     compared for exact equality (no floating point noise).

#include <chrono>
#include <iostream>
#include <vector>
using namespace std;

typedef vector<vector<long long>> Matrix;

const int CUTOFF = 64;

Matrix makeMatrix(int n) {
    return Matrix(n, vector<long long>(n, 0));
}

// ordinary O(n^3) multiplication (i-k-j order is a bit friendlier to the cache)
Matrix standardMultiply(const Matrix &A, const Matrix &B) {
    int n = A.size();
    Matrix C = makeMatrix(n);
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            long long temp = A[i][k];
            for (int j = 0; j < n; j++) {
                C[i][j] += temp * B[k][j];
            }
        }
    }
    return C;
}

Matrix add(const Matrix &A, const Matrix &B) {
    int n = A.size();
    Matrix C = makeMatrix(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = A[i][j] + B[i][j];
    return C;
}

Matrix subtract(const Matrix &A, const Matrix &B) {
    int n = A.size();
    Matrix C = makeMatrix(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = A[i][j] - B[i][j];
    return C;
}

// the recursive part, n must be a power of 2 here
Matrix strassenMultiply(const Matrix &A, const Matrix &B) {
    int n = A.size();

    // small enough, the plain method is faster
    if (n <= CUTOFF) return standardMultiply(A, B);

    int half = n / 2;

    // split A and B into four blocks each
    Matrix A11 = makeMatrix(half), A12 = makeMatrix(half), A21 = makeMatrix(half), A22 = makeMatrix(half);
    Matrix B11 = makeMatrix(half), B12 = makeMatrix(half), B21 = makeMatrix(half), B22 = makeMatrix(half);

    for (int i = 0; i < half; i++) {
        for (int j = 0; j < half; j++) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + half];
            A21[i][j] = A[i + half][j];
            A22[i][j] = A[i + half][j + half];

            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + half];
            B21[i][j] = B[i + half][j];
            B22[i][j] = B[i + half][j + half];
        }
    }

    // the seven products
    Matrix M1 = strassenMultiply(add(A11, A22), add(B11, B22));
    Matrix M2 = strassenMultiply(add(A21, A22), B11);
    Matrix M3 = strassenMultiply(A11, subtract(B12, B22));
    Matrix M4 = strassenMultiply(A22, subtract(B21, B11));
    Matrix M5 = strassenMultiply(add(A11, A12), B22);
    Matrix M6 = strassenMultiply(subtract(A21, A11), add(B11, B12));
    Matrix M7 = strassenMultiply(subtract(A12, A22), add(B21, B22));

    // put the four result blocks together
    Matrix C11 = add(subtract(add(M1, M4), M5), M7);
    Matrix C12 = add(M3, M5);
    Matrix C21 = add(M2, M4);
    Matrix C22 = add(subtract(add(M1, M3), M2), M6);

    Matrix C = makeMatrix(n);
    for (int i = 0; i < half; i++) {
        for (int j = 0; j < half; j++) {
            C[i][j] = C11[i][j];
            C[i][j + half] = C12[i][j];
            C[i + half][j] = C21[i][j];
            C[i + half][j + half] = C22[i][j];
        }
    }
    return C;
}

// pads to a power of two, multiplies, and trims back to the original size
Matrix multiplyAnySize(const Matrix &A, const Matrix &B) {
    int n = A.size();
    int size = 1;
    while (size < n) size *= 2;

    Matrix PA = makeMatrix(size), PB = makeMatrix(size);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            PA[i][j] = A[i][j];
            PB[i][j] = B[i][j];
        }
    }

    Matrix PC = strassenMultiply(PA, PB);

    Matrix C = makeMatrix(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = PC[i][j];
    return C;
}

Matrix randomMatrix(int n) {
    Matrix M = makeMatrix(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            M[i][j] = rand() % 10;
    return M;
}

int main() {
    srand(42);

    // tiny sanity check first so the result is easy to eyeball
    Matrix A = {{1, 2}, {3, 4}};
    Matrix B = {{5, 6}, {7, 8}};
    Matrix C = multiplyAnySize(A, B);
    cout << "Small example (expected [[19, 22], [43, 50]]):\n";
    for (int i = 0; i < 2; i++) cout << C[i][0] << " " << C[i][1] << "\n";
    cout << "\n";

    cout << "Size\tStandard (ms)\tStrassen (ms)\tResults match\n";

    int sizes[] = {64, 100, 128, 256, 300, 512, 1024};
    for (int n : sizes) {
        Matrix X = randomMatrix(n);
        Matrix Y = randomMatrix(n);

        auto t1 = chrono::high_resolution_clock::now();
        Matrix R1 = standardMultiply(X, Y);
        auto t2 = chrono::high_resolution_clock::now();

        Matrix R2 = multiplyAnySize(X, Y);
        auto t3 = chrono::high_resolution_clock::now();

        double standardTime = chrono::duration<double, milli>(t2 - t1).count();
        double strassenTime = chrono::duration<double, milli>(t3 - t2).count();

        cout << n << "\t" << standardTime << "\t\t" << strassenTime << "\t\t"
             << (R1 == R2 ? "yes" : "NO") << "\n";
    }

    return 0;
}
