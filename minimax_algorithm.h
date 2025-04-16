#ifndef MINIMAX_ALGORITHM_H
#define MINIMAX_ALGORITHM_H

#include <vector>
#include <utility>
#include <set>
#include <map>
#include <algorithm>
#include <tuple>
#include <iostream>

class MinimaxAlgorithm {
public:
    // Constructor
    MinimaxAlgorithm(std::pair<int, int> board_size = {12, 12}, int search_depth = 3, double attack_ratio = 1.0);
    
    // Get the best move for AI
    std::pair<int, int> get_next_move(const std::vector<std::pair<int, int>>& player_pieces, 
                               const std::vector<std::pair<int, int>>& opponent_pieces);
    
    // Get statistics
    std::map<std::string, int> get_statistics() const;

private:
    // Board dimensions
    int COLUMN;
    int ROW;
    int DEPTH;
    double ratio;
    
    // Statistics
    int cut_count;
    int search_count;
    
    // Game state
    std::vector<std::pair<int, int>> player_pieces;
    std::vector<std::pair<int, int>> opponent_pieces;
    std::vector<std::pair<int, int>> all_pieces;
    std::vector<std::pair<int, int>> all_positions;
    std::pair<int, int> next_move;
    
    // Shape scores for pattern evaluation
    std::vector<std::pair<int, std::vector<int>>> shape_score;
    
    // Algorithm methods
    int negamax(bool is_ai, int depth, int alpha, int beta);
    void order_moves(std::vector<std::pair<int, int>>& blank_list);
    bool has_neighbor(const std::pair<int, int>& point);
    int evaluation(bool is_ai);
    int cal_score(int m, int n, int x_direct, int y_direct, 
                 const std::vector<std::pair<int, int>>& enemy_list, 
                 const std::vector<std::pair<int, int>>& my_list, 
                 std::vector<std::tuple<int, std::vector<std::pair<int, int>>, std::pair<int, int>>>& score_all_arr);
    bool check_win(const std::vector<std::pair<int, int>>& pieces);
};

#endif // MINIMAX_ALGORITHM_H 