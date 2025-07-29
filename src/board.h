#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <utility>

enum Color { WHITE, BLACK };

class Board {
public:

    // Side to move
    Color sideToMove; 
    
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
    std::vector<int> generatePawnMoves  (int from) const;
    std::vector<int> generateKnightMoves(int from) const;
    std::vector<int> generateBishopMoves(int from) const;
    std::vector<int> generateRookMoves  (int from) const;
    std::vector<int> generateQueenMoves (int from) const;
    std::vector<int> generateKingMoves  (int from) const;

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
        Color   prevSide;
    };
    MoveRecord makeMove   (int from, int to);
    void       unmakeMove (const MoveRecord& rec);

    // Helper to convert 0-63 square index to coordinate
        static std::string idxToCoord(int idx) {
        char file = 'a' + (idx % 8);
        char rank = '1' + (idx / 8);
        return std::string{file, rank};
    }
    
    // Fast attack & legal-move API
    bool                              isSquareAttacked   (int sq, Color attacker)    const;
    bool                              isKingInCheck      (Color c)          const;
    std::vector<std::pair<int,int>>   generateAllLegalMoves() ;

    // Piece-to-bitboard mapper
    uint64_t& pieceBitboard(char piece);
};
