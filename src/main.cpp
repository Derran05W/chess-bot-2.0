#include "board.h"
#include "search.h"
#include <iostream>

int main() {
  Board board;
  board.print();

  while (true) {
    // Ask human for move, or parse UCI/console input...
    std::string from, to;
    std::cin >> from >> to;
    try {
      board.movePiece(from,to);
    } catch(...) {
      std::cerr << "Illegal move, try again\n";
      continue;
    }

    board.print();

    // Engine’s turn → search
    auto [eFrom, eTo] = Search::findBestMove(board, 5 /*maxDepth*/);
    board.movePiece(Board::idxToCoord(eFrom),
                    Board::idxToCoord(eTo));

    board.print();
  }
  return 0;
}
