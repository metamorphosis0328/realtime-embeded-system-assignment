#include "GomokuCoordinator.hpp"
#include <iostream>

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

    int human_count = ai.countPieces(human_player);
    int ai_count = ai.countPieces(ai_player);

    if (human_count == ai_count + 1)
    {
        auto [ai_row, ai_col] = ai.getBestMove();
        std::cout << "[AI] Decided move: (" << ai_row << ", " << ai_col << ")\n";

        if (armController)
        {
            std::cout << "[ARM] Executing move...\n";
            armController->gripNewPiece();
            armController->placePieceAt(ai_row, ai_col);
            armController->release();
            armController->resetServos();
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
