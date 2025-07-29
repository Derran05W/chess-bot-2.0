#pragma once
#include "board.h"

namespace Search {
  // Depth‐limited α-β search. Returns score *from* side‐to‐move’s perspective.
  int alphaBeta(Board& board, int depth, int α, int β);

  // convenience entry-point, e.g. iterative deepening
  std::pair<int,int> findBestMove(Board& board, int maxDepth);
}
