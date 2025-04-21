#ifndef GOMOKUCOORD_HPP
#define GOMOKUCOORD_HPP

#include "GomokuAI.hpp"
#include "GomokuVision.hpp"
#include "ArmController.hpp"
#include <string>

/**
 * @brief Coordinates the interactions between Vision, AI, and a robotic arm.
 *
 * This class receives new piece detections, updates the game state,
 * checks for wins, and triggers AI decisions when appropriate.
 */
class GomokuCoordinator : public PieceEventCallback
{
public:
    GomokuCoordinator(GomokuAI &ai, int ai_player, ArmController *arm);

    void onNewPieceDetected(int row, int col, const std::string &color) override;

private:
    GomokuAI &ai;
    const int ai_player;
    const int human_player;
    ArmController *armController;
};
#endif
