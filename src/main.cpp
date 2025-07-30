#include "board.h"
#include "search.h"
#include <iostream>

int main() {

  

  // Welcome message and instructions
  std::cout << "Welcome to the Chess Bot!\n";
  std::cout << "You can make moves in algebraic notation (e.g., e2 e4).\n";
  std::cout << "Type 'exit' to quit.\n";
  std::cout << "Enter your preferred difficulty level (1-5): ";

  // Read & Verify difficulty level ('Difficulty' just refers to the level of search depth)
  int difficulty;
  std::cin >> difficulty;
  if (difficulty < 1 || difficulty > 5) {
    std::cerr << "Invalid difficulty level. Defaulting to 3.\n";
    difficulty = 3;
  }
  std::cout << "Difficulty set to " << difficulty << ".\n";
  std::cout << "Let's start the game!\n";
  std::cout << "Your turn! Make a move:\n";

  // Initialize the chess board
  // This sets up the initial position of the chess pieces
  Board board;
  board.print();
  
  // Main game loop
  // This loop continues until the user decides to exit
  while (true) {

    // Ask human for move, or parse UCI/console input
    std::string from, to;
    std::cin >> from >> to;

    // Handle exit command
    if (from == "exit" || to == "exit") {
      std::cout << "Exiting the game. Goodbye!\n";
      break;
    }

    // Validate and execute the move, if it's invalid it restarts the loop
    try {
      board.movePiece(from,to);
    } catch(...) {
      std::cerr << "Illegal move, try again\n";
      continue;
    }
    
    // Print the board after the human move
    std::cout << "You played: " << from << " to " << to << "\n";
    board.print();

    // Engine’s turn → search
    auto [eFrom, eTo] = Search::findBestMove(board, difficulty);
    board.movePiece(Board::idxToCoord(eFrom),
                    Board::idxToCoord(eTo));

    std::cout << "Engine played: "
              << Board::idxToCoord(eFrom) << " to "
              << Board::idxToCoord(eTo) << "\n";
    board.print();
  }
  return 0;
}
