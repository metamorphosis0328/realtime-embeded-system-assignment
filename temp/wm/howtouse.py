# Import the encapsulated class
from minimax_algorithm import MinimaxAlgorithm

# Create an instance of the Minimax algorithm
# Parameter 1: Board size (columns, rows), default 12x12
# Parameter 2: Search depth, default 3 (odd numbers recommended)
# Parameter 3: Attack ratio, greater than 1 is more aggressive, less than 1 is more defensive
minimax = MinimaxAlgorithm(board_size=(12, 12), search_depth=3, attack_ratio=1)

# Usage in the game:
# player_pieces: List of AI's placed pieces [(x1,y1), (x2,y2), ...]
# opponent_pieces: List of opponent's placed pieces [(x1,y1), (x2,y2), ...]

# Example:
player_pieces = [(7, 7), (8, 8), (9, 9)]
opponent_pieces = [(7, 8), (8, 7)]

# Get the best position for the AI's next move
next_move = minimax.get_next_move(player_pieces, opponent_pieces)
print(f"The AI should place at position: {next_move}")

# Get algorithm statistics
stats = minimax.get_statistics()
print(f"Pruning count this search: {stats['cut_count']}")
print(f"Total searches this time: {stats['search_count']}")