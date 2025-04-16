# This work is heavily influenced by the following project:
# https://github.com/colingogogo/gobang_AI

class MinimaxAlgorithm:
    def __init__(self, board_size=(12, 12), search_depth=3, attack_ratio=1):
        """
        Initialize the Minimax algorithm
        :param board_size: Board size, default 12x12
        :param search_depth: Search depth, default 3 (must be odd)
        :param attack_ratio: Attack ratio (>1 is more aggressive, <1 is more defensive)
        """
        self.COLUMN, self.ROW = board_size
        self.DEPTH = search_depth
        self.ratio = attack_ratio
        
        # Statistics data
        self.cut_count = 0    # Pruning count
        self.search_count = 0 # Search count
        
        # Board data
        self.player_pieces = []   # AI pieces
        self.opponent_pieces = [] # Opponent pieces
        self.all_pieces = []      # All pieces
        self.all_positions = []   # All board positions
        self.next_move = [0, 0]   # Position AI will place next
        
        # Initialize all board positions
        for i in range(self.COLUMN+1):
            for j in range(self.ROW+1):
                self.all_positions.append((i, j))
        
        # Shape scoring table
        self.shape_score = [
            (50, (0, 1, 1, 0, 0)),
            (50, (0, 0, 1, 1, 0)),
            (200, (1, 1, 0, 1, 0)),
            (500, (0, 0, 1, 1, 1)),
            (500, (1, 1, 1, 0, 0)),
            (5000, (0, 1, 1, 1, 0)),
            (5000, (0, 1, 0, 1, 1, 0)),
            (5000, (0, 1, 1, 0, 1, 0)),
            (5000, (1, 1, 1, 0, 1)),
            (5000, (1, 1, 0, 1, 1)),
            (5000, (1, 0, 1, 1, 1)),
            (5000, (1, 1, 1, 1, 0)),
            (5000, (0, 1, 1, 1, 1)),
            (50000, (0, 1, 1, 1, 1, 0)),
            (99999999, (1, 1, 1, 1, 1))
        ]
    
    def get_next_move(self, player_pieces, opponent_pieces):
        """
        Get the best placement position for the AI's next move
        :param player_pieces: List of AI's placed pieces, each element is (x,y) coordinate
        :param opponent_pieces: List of opponent's placed pieces
        :return: (x,y) tuple representing the best placement position
        """
        self.player_pieces = player_pieces.copy()
        self.opponent_pieces = opponent_pieces.copy()
        self.all_pieces = player_pieces.copy() + opponent_pieces.copy()
        
        # Reset statistics
        self.cut_count = 0
        self.search_count = 0
        
        # Run the Minimax algorithm
        self.negamax(True, self.DEPTH, -99999999, 99999999)
        
        # Return the best move
        return (self.next_move[0], self.next_move[1])
    
    def get_statistics(self):
        """Get algorithm search statistics"""
        return {
            "cut_count": self.cut_count,
            "search_count": self.search_count
        }
    
    def negamax(self, is_ai, depth, alpha, beta):
        """
        Negamax search algorithm with alpha-beta pruning
        :param is_ai: Whether it is the AI's turn
        :param depth: Current search depth
        :param alpha: Alpha value
        :param beta: Beta value
        :return: Optimal score for the current position
        """
        # Check if the game is over or if the search depth is reached
        if self.check_win(self.player_pieces) or self.check_win(self.opponent_pieces) or depth == 0:
            return self.evaluation(is_ai)
        
        # Get all empty positions
        blank_list = list(set(self.all_positions).difference(set(self.all_pieces)))
        # Sort search order to improve pruning efficiency
        self.order_moves(blank_list)
        
        # Iterate through each candidate move
        for next_step in blank_list:
            self.search_count += 1
            
            # Skip positions without adjacent pieces (reduce computation)
            if not self.has_neighbor(next_step):
                continue
            
            # Simulate placing a piece
            if is_ai:
                self.player_pieces.append(next_step)
            else:
                self.opponent_pieces.append(next_step)
            self.all_pieces.append(next_step)
            
            # Recursive search
            value = -self.negamax(not is_ai, depth - 1, -beta, -alpha)
            
            # Undo the move
            if is_ai:
                self.player_pieces.remove(next_step)
            else:
                self.opponent_pieces.remove(next_step)
            self.all_pieces.remove(next_step)
            
            # Update the best value
            if value > alpha:
                if depth == self.DEPTH:
                    self.next_move[0] = next_step[0]
                    self.next_move[1] = next_step[1]
                
                # Alpha-beta pruning
                if value >= beta:
                    self.cut_count += 1
                    return beta
                alpha = value
        
        return alpha
    
    def order_moves(self, blank_list):
        """
        Sort possible placement positions, prioritizing those close to the last placed position
        :param blank_list: List of empty positions
        """
        if not self.all_pieces:
            return
            
        last_pt = self.all_pieces[-1]
        for i in range(-1, 2):
            for j in range(-1, 2):
                if i == 0 and j == 0:
                    continue
                    
                pos = (last_pt[0] + i, last_pt[1] + j)
                if pos in blank_list:
                    blank_list.remove(pos)
                    blank_list.insert(0, pos)
    
    def has_neighbor(self, point):
        """
        Check if a position has adjacent pieces
        :param point: Position coordinate
        :return: Boolean value
        """
        for i in range(-1, 2):
            for j in range(-1, 2):
                if i == 0 and j == 0:
                    continue
                    
                neighbor = (point[0] + i, point[1] + j)
                if neighbor in self.all_pieces:
                    return True
        return False
    
    def evaluation(self, is_ai):
        """
        Evaluate the score of the current position
        :param is_ai: Whether to evaluate from the AI's perspective
        :return: Score value
        """
        if is_ai:
            my_list = self.player_pieces
            enemy_list = self.opponent_pieces
        else:
            my_list = self.opponent_pieces
            enemy_list = self.player_pieces
        
        # Calculate the score for oneself
        score_all_arr = []
        my_score = 0
        for pt in my_list:
            m, n = pt
            my_score += self.cal_score(m, n, 0, 1, enemy_list, my_list, score_all_arr)
            my_score += self.cal_score(m, n, 1, 0, enemy_list, my_list, score_all_arr)
            my_score += self.cal_score(m, n, 1, 1, enemy_list, my_list, score_all_arr)
            my_score += self.cal_score(m, n, -1, 1, enemy_list, my_list, score_all_arr)
        
        # Calculate the score for the enemy
        score_all_arr_enemy = []
        enemy_score = 0
        for pt in enemy_list:
            m, n = pt
            enemy_score += self.cal_score(m, n, 0, 1, my_list, enemy_list, score_all_arr_enemy)
            enemy_score += self.cal_score(m, n, 1, 0, my_list, enemy_list, score_all_arr_enemy)
            enemy_score += self.cal_score(m, n, 1, 1, my_list, enemy_list, score_all_arr_enemy)
            enemy_score += self.cal_score(m, n, -1, 1, my_list, enemy_list, score_all_arr_enemy)
        
        # Total score = My score - Enemy score * ratio * 0.1
        return my_score - enemy_score * self.ratio * 0.1
    
    def cal_score(self, m, n, x_direct, y_direct, enemy_list, my_list, score_all_arr):
        """
        Calculate the score in a specific direction
        :param m, n: Piece position
        :param x_direct, y_direct: Direction vector
        :param enemy_list: Enemy pieces
        :param my_list: My pieces
        :param score_all_arr: Calculated score shapes
        :return: Score in that direction
        """
        add_score = 0
        max_score_shape = (0, None)
        
        # Check if this direction has been calculated
        for item in score_all_arr:
            for pt in item[1]:
                if m == pt[0] and n == pt[1] and x_direct == item[2][0] and y_direct == item[2][1]:
                    return 0
        
        # Scan in a specific direction to find shapes
        for offset in range(-5, 1):
            pos = []
            for i in range(6):
                point = (m + (i + offset) * x_direct, n + (i + offset) * y_direct)
                if point in enemy_list:
                    pos.append(2)
                elif point in my_list:
                    pos.append(1)
                else:
                    pos.append(0)
            
            tmp_shape5 = tuple(pos[:5])
            tmp_shape6 = tuple(pos)
            
            # Match shapes and score
            for score, shape in self.shape_score:
                if tmp_shape5 == shape or tmp_shape6 == shape:
                    if score > max_score_shape[0]:
                        max_score_shape = (score, tuple(
                            (m + (i + offset) * x_direct, n + (i + offset) * y_direct)
                            for i in range(5)
                        ), (x_direct, y_direct))
        
        # Calculate cross-score for shapes
        if max_score_shape[1] is not None:
            for item in score_all_arr:
                for pt1 in item[1]:
                    for pt2 in max_score_shape[1]:
                        if pt1 == pt2 and max_score_shape[0] > 10 and item[0] > 10:
                            add_score += item[0] + max_score_shape[0]
            
            score_all_arr.append(max_score_shape)
        
        return add_score + max_score_shape[0]
    
    def check_win(self, pieces):
        """
        Check for victory
        :param pieces: List of pieces
        :return: Boolean value
        """
        for m in range(self.COLUMN):
            for n in range(self.ROW):
                # Check horizontal direction
                if (n < self.ROW - 4 and 
                    (m, n) in pieces and 
                    (m, n + 1) in pieces and 
                    (m, n + 2) in pieces and 
                    (m, n + 3) in pieces and 
                    (m, n + 4) in pieces):
                    return True
                    
                # Check vertical direction
                elif (m < self.COLUMN - 4 and 
                      (m, n) in pieces and 
                      (m + 1, n) in pieces and 
                      (m + 2, n) in pieces and 
                      (m + 3, n) in pieces and 
                      (m + 4, n) in pieces):
                    return True
                    
                # Check right diagonal
                elif (m < self.COLUMN - 4 and n < self.ROW - 4 and
                      (m, n) in pieces and 
                      (m + 1, n + 1) in pieces and 
                      (m + 2, n + 2) in pieces and 
                      (m + 3, n + 3) in pieces and 
                      (m + 4, n + 4) in pieces):
                    return True
                    
                # Check left diagonal
                elif (m < self.COLUMN - 4 and n > 3 and
                      (m, n) in pieces and 
                      (m + 1, n - 1) in pieces and 
                      (m + 2, n - 2) in pieces and 
                      (m + 3, n - 3) in pieces and 
                      (m + 4, n - 4) in pieces):
                    return True
                    
        return False
