#pragma once
#include <vector>
#include <utility>

class GomokuAI {
public:
    GomokuAI(int size);
    void updateBoard(int row, int col, int player);
    std::pair<int, int> getBestMove();
    bool checkWin(int player);
    int countPieces(int player) const;

private:
    int size;
    std::vector<std::vector<int>> board;
    int evaluatePoint(int row, int col, int player);
};
