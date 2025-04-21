#include "GomokuVision.hpp"
#include "GomokuAI.hpp"

class MyCallback : public PieceEventCallback
{
public:
    MyCallback(GomokuAI &ai, int ai_player)
        : ai(ai), ai_player(ai_player), human_player(3 - ai_player) {}

    void onNewPieceDetected(int row, int col, const std::string &color) override
    {
        int player = (color == "black") ? 1 : 2;

        std::cout << "[Vision] Detected " << color << " piece at (" << row << ", " << col << ")\n";
        ai.updateBoard(row, col, player);

        if (ai.checkWin(player))
        {
            std::cout << "[Game Over] Player " << color << " wins!\n";
            return;
        }

        // Check if it's AI's turn
        int human_count = ai.countPieces(human_player);
        int ai_count = ai.countPieces(ai_player);

        if (human_count == ai_count + 1)
        {
            auto [ai_row, ai_col] = ai.getBestMove();
            std::cout << "[AI] Decided move: (" << ai_row << ", " << ai_col << ")\n";

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

private:
    GomokuAI &ai;
    const int ai_player;
    const int human_player;
};

int main()
{
    const int board_size = 9;
    GomokuAI ai(board_size);
    GomokuVision vision(2, 480, board_size);

    int ai_player = 2; // AI uses white stone
    MyCallback cb(ai, ai_player);

    vision.registerCallback(&cb);
    vision.run();
}