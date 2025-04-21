#include "GomokuVision.hpp"
#include "GomokuAI.hpp"

class MyCallback : public PieceEventCallback
{
public:
    MyCallback(GomokuAI &ai) : ai(ai) {}

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

        auto [ai_row, ai_col] = ai.getBestMove();
        std::cout << "[AI] Decided move: (" << ai_row << ", " << ai_col << ")\n";

        ai.updateBoard(ai_row, ai_col, 2);

        if (ai.checkWin(2))
        {
            std::cout << "[Game Over] AI wins!\n";
        }
    }

private:
    GomokuAI &ai;
};

int main()
{
    const int board_size = 9;

    GomokuAI ai(board_size);
    GomokuVision vision(0, 480, board_size);

    MyCallback cb(ai);
    vision.registerCallback(&cb);

    std::cout << "[System] Running Gomoku (9x9) with Vision + AI only\n";
    vision.run();

    return 0;
}