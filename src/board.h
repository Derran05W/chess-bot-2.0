// board.h
#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <utility>

class Board {
public:
    // Starting bitboards
    uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;

    // Constructor
    Board();

    // Occupancy helpers
    uint64_t getWhitePieces() const;
    uint64_t getBlackPieces() const;
    uint64_t getAllPieces()   const;

    // I/O
    void     print() const;
    char     getPieceAtSquare(int sq) const;

    // Algebraic moves
    void     movePiece(const std::string& from, const std::string& to);
    int      squareIndex(const std::string& coord) const;
    uint64_t squareMask (const std::string& coord) const;

    // Pseudo-legal generators
    std::vector<int> generatePawnMoves  (int from, bool isWhite) const;
    std::vector<int> generateKnightMoves(int from, bool isWhite) const;
    std::vector<int> generateBishopMoves(int from, bool isWhite) const;
    std::vector<int> generateRookMoves  (int from, bool isWhite) const;
    std::vector<int> generateQueenMoves (int from, bool isWhite) const;
    std::vector<int> generateKingMoves  (int from, bool isWhite) const;

    // Precomputed attack tables
    static const std::array<uint64_t,64> KNIGHT_ATTACKS;
    static const std::array<uint64_t,64> KING_ATTACKS;
    static const std::array<uint64_t,64> PAWN_ATTACKS_WHITE;
    static const std::array<uint64_t,64> PAWN_ATTACKS_BLACK;

    // Internals
    bool              onSameLine    (int from, int to, int dir) const;
    bool              onSameDiagonal(int from, int to, int dir) const;
    std::vector<int>  generatePseudoLegalMovesForSquare(int square) const;

    // Optimized make/unmake
    struct MoveRecord {
        int     from, to;
        uint64_t fromMask, toMask;
        char    movedPiece, capturedPiece;
    };
    MoveRecord makeMove   (int from, int to);
    void       unmakeMove (const MoveRecord& rec);

    // Fast attack & legal-move API
    bool                              isSquareAttacked   (int sq, bool byWhite) const;
    bool                              isKingInCheck      (bool isWhite)          const;
    std::vector<std::pair<int,int>>   generateAllLegalMoves(bool isWhite) ;

    // Piece-to-bitboard mapper
    uint64_t& pieceBitboard(char piece);
};
