#include "GomokuVision.hpp"
#include "GomokuAI.hpp"
#include "GomokuCoordinator.hpp"

int main()
{
    const int board_size = 9;
    GomokuAI ai(board_size);
    GomokuVision vision(2, 480, board_size);

    int ai_player = 2; // AI uses white stone
    GomokuCoordinator cb(ai, ai_player);

    vision.registerCallback(&cb);
    vision.run();
}