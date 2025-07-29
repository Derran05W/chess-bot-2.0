#include "search.h"
#include "eval.h"
#include <limits>
#include <cstdlib>
#include <iostream>


namespace Search {

  static void sortMoves(std::vector<std::pair<int,int>>& moves) {
    // Unused for now, but when visuals are implemented this will be useful
  }

  int alphaBeta(Board& board, int depth, int α, int β) {
    if (depth == 0) 
        return Eval::evaluate(board);

    auto moves = board.generateAllLegalMoves();
    if (moves.empty())
        return board.isKingInCheck(board.sideToMove)
               ? -99999
               : 0;

    sortMoves(moves);
    for (auto [from,to] : moves) {
        auto rec = board.makeMove(from,to);
        int score = -alphaBeta(board, depth-1, -β, -α);

        if (score >= β) {
            // β-cutoff: restore state _once_ and bail
            board.unmakeMove(rec);
            return β;
        }

        // no cutoff → restore and continue
        board.unmakeMove(rec);
        α = std::max(α, score);
    }
    return α;
}


  std::pair<int,int> findBestMove(Board& board, int maxDepth) {
    std::pair<int,int> bestMove{-1, -1};

        for (int d = 1; d <= maxDepth; ++d) {
            int α = -100000, β = +100000;
            int bestScoreThisDepth = std::numeric_limits<int>::min();
            std::pair<int,int> bestMoveThisDepth{-1,-1};

            auto moves = board.generateAllLegalMoves();
            sortMoves(moves);

            for (auto [from,to] : moves) {
                auto rec = board.makeMove(from,to);
                int score = -alphaBeta(board, d-1, -β, -α);
                board.unmakeMove(rec);

                if (score > bestScoreThisDepth) {
                    bestScoreThisDepth   = score;
                    bestMoveThisDepth    = {from,to};
                }
                α = std::max(α, score);
            }

            // logging used for debugging
            // std::cout << "Depth " << d << ": move "
            //         << Board::idxToCoord(bestMoveThisDepth.first)
            //         << Board::idxToCoord(bestMoveThisDepth.second)
            //         << " score " << bestScoreThisDepth << "\n";

            bestMove = bestMoveThisDepth;
        }

        return bestMove;
    }

} // namespace Search
