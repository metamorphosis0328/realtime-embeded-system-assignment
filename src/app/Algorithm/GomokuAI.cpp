#include "GomokuAI.hpp"

GomokuAI::GomokuAI(int size) : size(size), board(size, std::vector<int>(size, 0)) {}

void GomokuAI::updateBoard(int row, int col, int player) {
    board[row][col] = player;
}

bool GomokuAI::checkWin(int player) {
    const int dx[] = {1, 0, 1, 1};
    const int dy[] = {0, 1, -1, 1};

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            for (int d = 0; d < 4; d++) {
                int cnt = 0;
                for (int k = 0; k < 5; k++) {
                    int x = i + k * dx[d];
                    int y = j + k * dy[d];
                    if (x >= 0 && x < size && y >= 0 && y < size && board[x][y] == player)
                        cnt++;
                    else break;
                }
                if (cnt == 5) return true;
            }
    return false;
}

int GomokuAI::evaluatePoint(int row, int col, int player) {
     int score = 0;
    const int dx[] = {1, 0, 1, 1};
    const int dy[] = {0, 1, -1, 1};

    for (int d = 0; d < 4; ++d) {
        int count = 1;
        for (int step = 1; step <= 4; step++) {
            int x = row + dx[d] * step;
            int y = col + dy[d] * step;
            if (x >= 0 && x < size && y >= 0 && y < size && board[x][y] == player)
                count++;
            else break;
        }
        score += count * count;
    }
    return score;
}

std::pair<int, int> GomokuAI::getBestMove() {
    int best_score = -1;
    std::pair<int, int> best_move = {-1, -1};

    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (board[i][j] == 0) {
                int score = evaluatePoint(i, j, 2) + evaluatePoint(i, j, 1) / 2;
                if (score > best_score) {
                    best_score = score;
                    best_move = {i, j};
                }
            }

    return best_move;
}
