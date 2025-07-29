// eval.cpp
#include "eval.h"
#include "board.h"

// Calculates how many 1 bits in the 64 bit number (Counts pieces on the board)
static inline int popcount(uint64_t b) {
    return __builtin_popcountll(b);
}
static inline int pop_lsb(uint64_t& b) {
    int sq = __builtin_ctzll(b);
    b &= b - 1;
    return sq;
}

namespace Eval {


int evaluate(const Board& board) {
    int sc = materialScore(board)
        + positionScore(board);
    // always return the score **from** the side‐to‐move’s perspective
    return (board.sideToMove == WHITE) ? sc : -sc;
}


// Calculate the material score of the board, of White - Black
int materialScore(const Board& board) {
    int score = 0;

    // Pawns
    score += popcount(board.whitePawns) * PieceValue[0];
    score -= popcount(board.blackPawns) * PieceValue[0];

    // Knights
    score += popcount(board.whiteKnights) * PieceValue[1];
    score -= popcount(board.blackKnights) * PieceValue[1];

    // Bishops
    score += popcount(board.whiteBishops) * PieceValue[2];
    score -= popcount(board.blackBishops) * PieceValue[2];

    // Rooks
    score += popcount(board.whiteRooks) * PieceValue[3];
    score -= popcount(board.blackRooks) * PieceValue[3];

    // Queens
    score += popcount(board.whiteQueens) * PieceValue[4];
    score -= popcount(board.blackQueens) * PieceValue[4];

    // Kings 
    score += popcount(board.whiteKing) * PieceValue[5];
    score -= popcount(board.blackKing) * PieceValue[5];

    return score;
}

// Calculate the position score of the board, using piece-square tables representing a mid game position.
// Currently, this is simplified to not consider advanced concepts like king safety or pawn structure, or whether
// the game is early, mid, or late game. 
int positionScore(const Board& board) {
    int score = 0;
    uint64_t bb;

    // White pieces
    bb = board.whitePawns;
    while (bb)                score += PST_PAWN[pop_lsb(bb)];

    bb = board.whiteKnights;
    while (bb)                score += PST_KNIGHT[pop_lsb(bb)];

    bb = board.whiteBishops;
    while (bb)                score += PST_BISHOP[pop_lsb(bb)];

    bb = board.whiteRooks;
    while (bb)                score += PST_ROOK[pop_lsb(bb)];

    bb = board.whiteQueens;
    while (bb)                score += PST_QUEEN[pop_lsb(bb)];

    bb = board.whiteKing;
    while (bb)                score += PST_KING[pop_lsb(bb)];

    // Black pieces (mirror the square with sq^56)
    bb = board.blackPawns;
    while (bb)                score -= PST_PAWN[pop_lsb(bb) ^ 56];

    bb = board.blackKnights;
    while (bb)                score -= PST_KNIGHT[pop_lsb(bb) ^ 56];

    bb = board.blackBishops;
    while (bb)                score -= PST_BISHOP[pop_lsb(bb) ^ 56];

    bb = board.blackRooks;
    while (bb)                score -= PST_ROOK[pop_lsb(bb) ^ 56];

    bb = board.blackQueens;
    while (bb)                score -= PST_QUEEN[pop_lsb(bb) ^ 56];

    bb = board.blackKing;
    while (bb)                score -= PST_KING[pop_lsb(bb) ^ 56];

    return score;
}

} // namespace Eval
