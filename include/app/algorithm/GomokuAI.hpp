#ifndef GOMOKUAI_HPP
#define GOMOKUAI_HPP

#include <vector>
#include <utility>
#include "MinimaxAlgorithm.hpp"

class GomokuAI
{
public:
    GomokuAI(int size);
    void updateBoard(int row, int col, int player);
    std::pair<int, int> getBestMove();
    bool checkWin(int player);
    int countPieces(int player) const;

private:
    int size;
    std::vector<std::vector<int>> board;
    MinimaxAlgorithm minimax;
};
#endif
