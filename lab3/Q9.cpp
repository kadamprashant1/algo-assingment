// Given a chessboard with dimensions n × n, where n = 2k and k > 0 . On this chessboard, exactly one square is missing (See Figure 2). Develop an algorithm
// to cover the entire chessboard (excluding the missing square) using L-shaped triominoes.

// A L-shaped triomino is a 2 × 2 block with one cell of size 1 × 1 missing (SeeFigure 1).
// Constraints:
// i. Two triominoes should not overlap.
// ii. Triminoes should not cover defective square.
// iii. Triominoes should cover all other squares.



#include <bits/stdc++.h>
using namespace std;

int tileId = 0;                 // running counter, one id per triomino
vector<vector<int>> board;      // board[r][c] = triomino id (0 = missing cell)

// Tiles the square whose top-left corner is (top, left) with side `size`.
// The missing cell inside this square is (missR, missC).
void tile(int top, int left, int missR, int missC, int size) {
    if (size == 1) return;      // single cell is the missing one, nothing to do

    int id = ++tileId;          // id for the triomino placed at this level
    int half = size / 2;
    int midR = top + half;      // first row of the lower half
    int midC = left + half;     // first column of the right half

    // Centre cells of each quadrant, adjacent to the middle of the square.
    // Index: 0 = top-left, 1 = top-right, 2 = bottom-left, 3 = bottom-right
    int cr[4] = {midR - 1, midR - 1, midR,     midR};
    int cc[4] = {midC - 1, midC,     midC - 1, midC};

    // Which quadrant holds the missing cell?
    int q = (missR >= midR ? 2 : 0) + (missC >= midC ? 1 : 0);

    // Top-left corner and missing cell for each quadrant's subproblem
    int qTop[4]  = {top,  top,  midR, midR};
    int qLeft[4] = {left, midC, left, midC};
    int mr[4], mc[4];

    for (int i = 0; i < 4; i++) {
        if (i == q) {
            mr[i] = missR;      // real missing cell
            mc[i] = missC;
        } else {
            mr[i] = cr[i];      // cell covered by the central triomino
            mc[i] = cc[i];
            board[cr[i]][cc[i]] = id;
        }
    }

    for (int i = 0; i < 4; i++)
        tile(qTop[i], qLeft[i], mr[i], mc[i], half);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    int k;
    cout << "Enter k (board size n = 2^k): " << flush;
    cin >> k;
    if (k <= 0) {
        cout << "k must be > 0\n";
        return 1;
    }

    int n = 1 << k;
    int mr, mc;
    cout << "Enter missing square (row col, 0-indexed, 0.." << n - 1 << "): "<< flush;
    cin >> mr >> mc;
    if (mr < 0 || mr >= n || mc < 0 || mc >= n) {
        cout << "Invalid position\n";
        return 1;
    }

    board.assign(n, vector<int>(n, 0));
    tile(0, 0, mr, mc, n);

    cout << "\nTiling (0 = missing square, same number = same triomino):\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == mr && j == mc) cout << setw(4) << "X";
            else                    cout << setw(4) << board[i][j];
        }
        cout << "\n";
    }
    return 0;
}