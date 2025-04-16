#include "minimax_algorithm.h"
#include <iostream>
#include <algorithm>

// Constructor implementation
MinimaxAlgorithm::MinimaxAlgorithm(std::pair<int, int> board_size, int search_depth, double attack_ratio) {
    // Initialize basic parameters
    COLUMN = board_size.first;
    ROW = board_size.second;
    DEPTH = search_depth;
    ratio = attack_ratio;
    
    // Initialize statistics
    cut_count = 0;
    search_count = 0;
    
    // Initialize next move
    next_move = {0, 0};
    
    // Initialize all possible board positions
    for (int i = 0; i <= COLUMN; i++) {
        for (int j = 0; j <= ROW; j++) {
            all_positions.push_back({i, j});
        }
    }
    
    // Initialize shape scoring table
    shape_score = {
        {50, {0, 1, 1, 0, 0}},
        {50, {0, 0, 1, 1, 0}},
        {200, {1, 1, 0, 1, 0}},
        {500, {0, 0, 1, 1, 1}},
        {500, {1, 1, 1, 0, 0}},
        {5000, {0, 1, 1, 1, 0}},
        {5000, {0, 1, 0, 1, 1, 0}},
        {5000, {0, 1, 1, 0, 1, 0}},
        {5000, {1, 1, 1, 0, 1}},
        {5000, {1, 1, 0, 1, 1}},
        {5000, {1, 0, 1, 1, 1}},
        {5000, {1, 1, 1, 1, 0}},
        {5000, {0, 1, 1, 1, 1}},
        {50000, {0, 1, 1, 1, 1, 0}},
        {99999999, {1, 1, 1, 1, 1}}
    };
}

std::pair<int, int> MinimaxAlgorithm::get_next_move(
    const std::vector<std::pair<int, int>>& player_pieces_input, 
    const std::vector<std::pair<int, int>>& opponent_pieces_input
) {
    // Copy the input pieces
    player_pieces = player_pieces_input;
    opponent_pieces = opponent_pieces_input;
    
    // Create all_pieces by combining player and opponent pieces
    all_pieces = player_pieces;
    all_pieces.insert(all_pieces.end(), opponent_pieces.begin(), opponent_pieces.end());
    
    // Reset statistics
    cut_count = 0;
    search_count = 0;
    
    // Run the Minimax algorithm
    negamax(true, DEPTH, -99999999, 99999999);
    
    // Return the best move
    return next_move;
}

std::map<std::string, int> MinimaxAlgorithm::get_statistics() const {
    return {
        {"cut_count", cut_count},
        {"search_count", search_count}
    };
}

int MinimaxAlgorithm::negamax(bool is_ai, int depth, int alpha, int beta) {
    // Check if the game is over or if the search depth is reached
    if (check_win(player_pieces) || check_win(opponent_pieces) || depth == 0) {
        return evaluation(is_ai);
    }
    
    // Get all empty positions by finding differences between all positions and placed pieces
    std::vector<std::pair<int, int>> blank_list;
    for (const auto& pos : all_positions) {
        if (std::find(all_pieces.begin(), all_pieces.end(), pos) == all_pieces.end()) {
            blank_list.push_back(pos);
        }
    }
    
    // Sort search order to improve pruning efficiency
    order_moves(blank_list);
    
    // Iterate through each candidate move
    for (const auto& next_step : blank_list) {
        search_count++;
        
        // Skip positions without adjacent pieces (reduce computation)
        if (!has_neighbor(next_step)) {
            continue;
        }
        
        // Simulate placing a piece
        if (is_ai) {
            player_pieces.push_back(next_step);
        } else {
            opponent_pieces.push_back(next_step);
        }
        all_pieces.push_back(next_step);
        
        // Recursive search
        int value = -negamax(!is_ai, depth - 1, -beta, -alpha);
        
        // Undo the move
        if (is_ai) {
            player_pieces.pop_back();
        } else {
            opponent_pieces.pop_back();
        }
        all_pieces.pop_back();
        
        // Update the best value
        if (value > alpha) {
            if (depth == DEPTH) {
                next_move = next_step;
            }
            
            // Alpha-beta pruning
            if (value >= beta) {
                cut_count++;
                return beta;
            }
            alpha = value;
        }
    }
    
    return alpha;
}

void MinimaxAlgorithm::order_moves(std::vector<std::pair<int, int>>& blank_list) {
    if (all_pieces.empty()) {
        return;
    }
    
    auto last_pt = all_pieces.back();
    
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            
            std::pair<int, int> pos = {last_pt.first + i, last_pt.second + j};
            auto it = std::find(blank_list.begin(), blank_list.end(), pos);
            
            if (it != blank_list.end()) {
                blank_list.erase(it);
                blank_list.insert(blank_list.begin(), pos);
            }
        }
    }
}

bool MinimaxAlgorithm::has_neighbor(const std::pair<int, int>& point) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            
            std::pair<int, int> neighbor = {point.first + i, point.second + j};
            if (std::find(all_pieces.begin(), all_pieces.end(), neighbor) != all_pieces.end()) {
                return true;
            }
        }
    }
    return false;
}

int MinimaxAlgorithm::evaluation(bool is_ai) {
    std::vector<std::pair<int, int>> my_list;
    std::vector<std::pair<int, int>> enemy_list;
    
    if (is_ai) {
        my_list = player_pieces;
        enemy_list = opponent_pieces;
    } else {
        my_list = opponent_pieces;
        enemy_list = player_pieces;
    }
    
    // Calculate the score for oneself
    std::vector<std::tuple<int, std::vector<std::pair<int, int>>, std::pair<int, int>>> score_all_arr;
    int my_score = 0;
    
    for (const auto& pt : my_list) {
        int m = pt.first;
        int n = pt.second;
        my_score += cal_score(m, n, 0, 1, enemy_list, my_list, score_all_arr);
        my_score += cal_score(m, n, 1, 0, enemy_list, my_list, score_all_arr);
        my_score += cal_score(m, n, 1, 1, enemy_list, my_list, score_all_arr);
        my_score += cal_score(m, n, -1, 1, enemy_list, my_list, score_all_arr);
    }
    
    // Calculate the score for the enemy
    std::vector<std::tuple<int, std::vector<std::pair<int, int>>, std::pair<int, int>>> score_all_arr_enemy;
    int enemy_score = 0;
    
    for (const auto& pt : enemy_list) {
        int m = pt.first;
        int n = pt.second;
        enemy_score += cal_score(m, n, 0, 1, my_list, enemy_list, score_all_arr_enemy);
        enemy_score += cal_score(m, n, 1, 0, my_list, enemy_list, score_all_arr_enemy);
        enemy_score += cal_score(m, n, 1, 1, my_list, enemy_list, score_all_arr_enemy);
        enemy_score += cal_score(m, n, -1, 1, my_list, enemy_list, score_all_arr_enemy);
    }
    
    // Total score = My score - Enemy score * ratio * 0.1
    return my_score - static_cast<int>(enemy_score * ratio * 0.1);
}

int MinimaxAlgorithm::cal_score(
    int m, int n, int x_direct, int y_direct, 
    const std::vector<std::pair<int, int>>& enemy_list,
    const std::vector<std::pair<int, int>>& my_list,
    std::vector<std::tuple<int, std::vector<std::pair<int, int>>, std::pair<int, int>>>& score_all_arr
) {
    int add_score = 0;
    std::pair<int, std::vector<std::pair<int, int>>> max_score_shape = {0, {}};
    std::pair<int, int> direction = {x_direct, y_direct};
    
    // Check if this direction has been calculated
    for (const auto& item : score_all_arr) {
        for (const auto& pt : std::get<1>(item)) {
            if (m == pt.first && n == pt.second && 
                x_direct == std::get<2>(item).first && 
                y_direct == std::get<2>(item).second) {
                return 0;
            }
        }
    }
    
    // Scan in a specific direction to find shapes
    for (int offset = -5; offset < 1; offset++) {
        std::vector<int> pos;
        std::vector<std::pair<int, int>> positions;
        
        for (int i = 0; i < 6; i++) {
            std::pair<int, int> point = {m + (i + offset) * x_direct, n + (i + offset) * y_direct};
            positions.push_back(point);
            
            if (std::find(enemy_list.begin(), enemy_list.end(), point) != enemy_list.end()) {
                pos.push_back(2);
            } else if (std::find(my_list.begin(), my_list.end(), point) != my_list.end()) {
                pos.push_back(1);
            } else {
                pos.push_back(0);
            }
        }
        
        std::vector<int> tmp_shape5(pos.begin(), pos.begin() + 5);
        
        // Match shapes and score
        for (const auto& shape_pair : shape_score) {
            int score = shape_pair.first;
            const auto& shape = shape_pair.second;
            
            bool matched = false;
            
            // Check for 5-length pattern
            if (shape.size() == 5) {
                matched = std::equal(shape.begin(), shape.end(), tmp_shape5.begin());
            } 
            // Check for 6-length pattern
            else if (shape.size() == 6) {
                matched = std::equal(shape.begin(), shape.end(), pos.begin());
            }
            
            if (matched && score > max_score_shape.first) {
                std::vector<std::pair<int, int>> shape_positions;
                for (int i = 0; i < 5; i++) {
                    shape_positions.push_back({m + (i + offset) * x_direct, n + (i + offset) * y_direct});
                }
                max_score_shape = {score, shape_positions};
            }
        }
    }
    
    // Calculate cross-score for shapes
    if (!max_score_shape.second.empty()) {
        for (const auto& item : score_all_arr) {
            for (const auto& pt1 : std::get<1>(item)) {
                for (const auto& pt2 : max_score_shape.second) {
                    if (pt1 == pt2 && max_score_shape.first > 10 && std::get<0>(item) > 10) {
                        add_score += std::get<0>(item) + max_score_shape.first;
                    }
                }
            }
        }
        
        score_all_arr.push_back({max_score_shape.first, max_score_shape.second, direction});
    }
    
    return add_score + max_score_shape.first;
}

bool MinimaxAlgorithm::check_win(const std::vector<std::pair<int, int>>& pieces) {
    for (int m = 0; m < COLUMN; m++) {
        for (int n = 0; n < ROW; n++) {
            // Check horizontal direction
            if (n < ROW - 4 &&
                std::find(pieces.begin(), pieces.end(), std::make_pair(m, n)) != pieces.end() &&
                std::find(pieces.begin(), pieces.end(), std::make_pair(m, n + 1)) != pieces.end() &&
                std::find(pieces.begin(), pieces.end(), std::make_pair(m, n + 2)) != pieces.end() &&
                std::find(pieces.begin(), pieces.end(), std::make_pair(m, n + 3)) != pieces.end() &&
                std::find(pieces.begin(), pieces.end(), std::make_pair(m, n + 4)) != pieces.end()) {
                return true;
            }
            
            // Check vertical direction
            else if (m < COLUMN - 4 &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m, n)) != pieces.end() &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m + 1, n)) != pieces.end() &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m + 2, n)) != pieces.end() &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m + 3, n)) != pieces.end() &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m + 4, n)) != pieces.end()) {
                return true;
            }
            
            // Check right diagonal
            else if (m < COLUMN - 4 && n < ROW - 4 &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m, n)) != pieces.end() &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m + 1, n + 1)) != pieces.end() &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m + 2, n + 2)) != pieces.end() &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m + 3, n + 3)) != pieces.end() &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m + 4, n + 4)) != pieces.end()) {
                return true;
            }
            
            // Check left diagonal
            else if (m < COLUMN - 4 && n > 3 &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m, n)) != pieces.end() &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m + 1, n - 1)) != pieces.end() &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m + 2, n - 2)) != pieces.end() &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m + 3, n - 3)) != pieces.end() &&
                     std::find(pieces.begin(), pieces.end(), std::make_pair(m + 4, n - 4)) != pieces.end()) {
                return true;
            }
        }
    }
    
    return false;
} 