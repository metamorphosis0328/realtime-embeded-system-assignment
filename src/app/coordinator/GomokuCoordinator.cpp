#include "GomokuCoordinator.hpp"
#include <iostream>
#include <thread>

GomokuCoordinator::GomokuCoordinator(GomokuAI &ai, int ai_player, ArmController *arm)
    : ai(ai), ai_player(ai_player), human_player(3 - ai_player), armController(arm) {}

void GomokuCoordinator::onNewPieceDetected(int row, int col, const std::string &color)
{
    int player = (color == "black") ? 1 : 2;

    std::cout << "[Vision] Detected " << color << " piece at (" << row << ", " << col << ")\n";
    ai.updateBoard(row, col, player);

    if (ai.checkWin(player))
    {
        std::cout << "[Game Over] Player " << color << " wins!\n";
        return;
    }

    int black_count = ai.countPieces(1);
    int white_count = ai.countPieces(2);
    int total_pieces = black_count + white_count;

    // It's AI's turn if:
    //     total pieces are even and AI is black (goes first)
    //     or total pieces are odd and AI is white (goes second, by default)
    bool is_ai_turn = ( (total_pieces % 2 == 0 && ai_player == 1) ||
                        (total_pieces % 2 == 1 && ai_player == 2) );
    if (is_ai_turn)
    {
        auto [ai_row, ai_col] = ai.getBestMove();
        std::cout << "[AI] Decided move: (" << ai_row << ", " << ai_col << ")\n";

        if (armController)
        {
            std::cout << "[ARM] Executing move...\n";
            armController->enqueueMove(ai_row, ai_col);
        }

        ai.updateBoard(ai_row, ai_col, ai_player);

        if (ai.checkWin(ai_player))
        {
            std::cout << "[Game Over] AI wins!\n";
        }
    }
    else
    {
        std::cout << "[AI] Not my turn yet.\n";
    }
}
