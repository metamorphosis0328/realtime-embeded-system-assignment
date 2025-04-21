#include "GomokuAI.hpp"

GomokuAI::GomokuAI(int size)
    : size(size), board(size, std::vector<int>(size, 0)),
      minimax({size, size}, 2 /* search depth */, 1.0 /* attack-defense ratio */) {}

void GomokuAI::updateBoard(int row, int col, int player)
{
    board[row][col] = player;
}

bool GomokuAI::checkWin(int player)
{
    std::vector<std::pair<int, int>> pieces;
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (board[i][j] == player)
                pieces.emplace_back(i, j);
    return minimax.check_win(pieces);
}

int GomokuAI::countPieces(int player) const
{
    int count = 0;
    for (const auto &row : board)
        for (int cell : row)
            if (cell == player)
                count++;
    return count;
}

std::pair<int, int> GomokuAI::getBestMove()
{
    std::vector<std::pair<int, int>> ai_pieces;
    std::vector<std::pair<int, int>> human_pieces;

    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
        {
            if (board[i][j] == 2)
                ai_pieces.emplace_back(i, j);
            else if (board[i][j] == 1)
                human_pieces.emplace_back(i, j);
        }

    return minimax.get_next_move(ai_pieces, human_pieces);
}
