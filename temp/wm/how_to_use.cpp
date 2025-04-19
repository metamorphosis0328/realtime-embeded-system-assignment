#include <iostream>
#include <vector>
#include "minimax_algorithm.h"

int main() {
    // Create an instance of the Minimax algorithm
    // Parameter 1: Board size (columns, rows), default 12x12
    // Parameter 2: Search depth, default 3 (odd numbers recommended)
    // Parameter 3: Attack ratio, greater than 1 is more aggressive, less than 1 is more defensive
    MinimaxAlgorithm minimax({12, 12}, 3, 1.0);

    // Usage in the game:
    // player_pieces: List of AI's placed pieces [(x1,y1), (x2,y2), ...]
    // opponent_pieces: List of opponent's placed pieces [(x1,y1), (x2,y2), ...]

    // Example:
    std::vector<std::pair<int, int>> player_pieces = {{7, 7}, {8, 8}, {9, 9}};
    std::vector<std::pair<int, int>> opponent_pieces = {{7, 8}, {8, 7}};

    // Get the best position for the AI's next move
    auto next_move = minimax.get_next_move(player_pieces, opponent_pieces);
    std::cout << "The AI should place at position: (" << next_move.first << ", " << next_move.second << ")" << std::endl;

    // Get algorithm statistics
    auto stats = minimax.get_statistics();
    std::cout << "Pruning count this search: " << stats["cut_count"] << std::endl;
    std::cout << "Total searches this time: " << stats["search_count"] << std::endl;

    return 0;
} 