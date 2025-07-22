// Optimized move generation and check detection for Board
#include "board.h"
#include <array>
#include <vector>
#include <utility>
#include <iostream>
#include <stdexcept>

// Initialize precomputed attack tables
const std::array<uint64_t,64> Board::KNIGHT_ATTACKS = [](){
    std::array<uint64_t,64> tbl{};
    for(int sq=0; sq<64; ++sq){
        int f = sq % 8, r = sq / 8;
        uint64_t m = 0;
        const int offs[8][2] = {{1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2}};
        for(auto &o: offs){
            int nf = f + o[0], nr = r + o[1];
            if(nf>=0 && nf<8 && nr>=0 && nr<8)
                m |= 1ULL << (nr*8 + nf);
        }
        tbl[sq] = m;
    }
    return tbl;
}();
const std::array<uint64_t,64> Board::KING_ATTACKS = [](){
    std::array<uint64_t,64> tbl{};
    for(int sq=0; sq<64; ++sq){
        int f = sq % 8, r = sq / 8;
        uint64_t m = 0;
        for(int df=-1; df<=1; ++df) for(int dr=-1; dr<=1; ++dr){
            if(df||dr){
                int nf=f+df, nr=r+dr;
                if(nf>=0 && nf<8 && nr>=0 && nr<8)
                    m |= 1ULL << (nr*8 + nf);
            }
        }
        tbl[sq] = m;
    }
    return tbl;
}();
const std::array<uint64_t,64> Board::PAWN_ATTACKS_WHITE = [](){
    std::array<uint64_t,64> tbl{};
    for(int sq=0; sq<64; ++sq){
        int f=sq%8, r=sq/8; uint64_t m=0;
        if(r<7){
            if(f>0)   m |= 1ULL<<((r+1)*8 + (f-1));
            if(f<7)   m |= 1ULL<<((r+1)*8 + (f+1));
        }
        tbl[sq]=m;
    }
    return tbl;
}();
const std::array<uint64_t,64> Board::PAWN_ATTACKS_BLACK = [](){
    std::array<uint64_t,64> tbl{};
    for(int sq=0; sq<64; ++sq){
        int f=sq%8, r=sq/8; uint64_t m=0;
        if(r>0){
            if(f>0)   m |= 1ULL<<((r-1)*8 + (f-1));
            if(f<7)   m |= 1ULL<<((r-1)*8 + (f+1));
        }
        tbl[sq]=m;
    }
    return tbl;
}();

struct MoveRecord {
    int from, to;
    uint64_t fromMask, toMask;
    char movedPiece, capturedPiece;
};

// Constructor initializes the board with standard starting positions
Board::Board() {
    whitePawns   = 0x000000000000FF00ULL;
    whiteKnights = 0x0000000000000042ULL;
    whiteBishops = 0x0000000000000024ULL;
    whiteRooks   = 0x0000000000000081ULL;
    whiteQueens  = 0x0000000000000008ULL;
    whiteKing    = 0x0000000000000010ULL;

    blackPawns   = 0x00FF000000000000ULL;
    blackKnights = 0x4200000000000000ULL;
    blackBishops = 0x2400000000000000ULL;
    blackRooks   = 0x8100000000000000ULL;
    blackQueens  = 0x0800000000000000ULL;
    blackKing    = 0x1000000000000000ULL;
}

// Helpers to get all pieces for white, black, or both
uint64_t Board::getWhitePieces() const {
    return whitePawns
         | whiteKnights
         | whiteBishops
         | whiteRooks
         | whiteQueens
         | whiteKing;
}
uint64_t Board::getBlackPieces() const {
    return blackPawns
         | blackKnights
         | blackBishops
         | blackRooks
         | blackQueens
         | blackKing;
}
uint64_t Board::getAllPieces() const {
    return getWhitePieces() | getBlackPieces();
}

// Print the board in a human-readable format
void Board::print() const {
    std::cout << "\n  a b c d e f g h\n";
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << " ";
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            std::cout << getPieceAtSquare(sq) << " ";
        }
        std::cout << rank + 1 << "\n";
    }
    std::cout << "  a b c d e f g h\n\n";
}
char Board::getPieceAtSquare(int sq) const {
    uint64_t mask = 1ULL << sq;
    if (whitePawns   & mask) return 'P';
    if (whiteKnights & mask) return 'N';
    if (whiteBishops & mask) return 'B';
    if (whiteRooks   & mask) return 'R';
    if (whiteQueens  & mask) return 'Q';
    if (whiteKing    & mask) return 'K';

    if (blackPawns   & mask) return 'p';
    if (blackKnights & mask) return 'n';
    if (blackBishops & mask) return 'b';
    if (blackRooks   & mask) return 'r';
    if (blackQueens  & mask) return 'q';
    if (blackKing    & mask) return 'k';

    return '.';
}

// Make a move in-place and return the record needed to undo it
#include <stdexcept>

Board::MoveRecord Board::makeMove(int from, int to) {
    // 1Verify the move is valid
    char pc = getPieceAtSquare(from);
    if (pc == '.' || ((pc >= 'A' && pc <= 'Z') != true)) {
        throw std::invalid_argument("makeMove: no white piece at source");
    }
    auto pseudos = generatePseudoLegalMovesForSquare(from);
    bool found = false;
    for (int tgt : pseudos) if (tgt == to) { found = true; break; }
    if (!found) {
        throw std::invalid_argument("makeMove: target not in pseudo-legal moves");
    }

    // Perform the Move (And Capture if needed)
    MoveRecord rec;
    rec.from         = from;
    rec.to           = to;
    rec.fromMask     = 1ULL << from;
    rec.toMask       = 1ULL << to;
    rec.movedPiece   = pc;
    rec.capturedPiece= getPieceAtSquare(to);

    // Clear the captured piece if any
    whitePawns   &= ~rec.toMask;
    whiteKnights &= ~rec.toMask;
    whiteBishops &= ~rec.toMask;
    whiteRooks   &= ~rec.toMask;
    whiteQueens  &= ~rec.toMask;
    whiteKing    &= ~rec.toMask;
    blackPawns   &= ~rec.toMask;
    blackKnights &= ~rec.toMask;
    blackBishops &= ~rec.toMask;
    blackRooks   &= ~rec.toMask;
    blackQueens  &= ~rec.toMask;
    blackKing    &= ~rec.toMask;

    // relocate
    uint64_t &bb = pieceBitboard(rec.movedPiece);
    bb &= ~rec.fromMask;
    bb |= rec.toMask;

    // Verify the move does not leave the king in check
    bool isWhite = (rec.movedPiece >= 'A' && rec.movedPiece <= 'Z');
    if (isKingInCheck(isWhite)) {
        // Undo and throw if it does
        unmakeMove(rec);
        throw std::invalid_argument("makeMove: move would leave king in check");
    }

    return rec;
}


// Undo a previously made move using the record
void Board::unmakeMove(const MoveRecord &rec) {
    uint64_t &bb = pieceBitboard(rec.movedPiece);
    bb &= ~rec.toMask;
    bb |= rec.fromMask;

    if (rec.capturedPiece != '.') {
        uint64_t &cb = pieceBitboard(rec.capturedPiece);
        cb |= rec.toMask;
    }
}

//Helper Functions for square indexing and masking
 int Board::squareIndex(const std::string& coord) const  {
    // rank '1' → 0, file 'a' → 0
    return (coord[1] - '1') * 8 + (coord[0] - 'a');
}

// Produce a bitmask with only that square set
 uint64_t Board::squareMask(const std::string& coord) const  {
    return 1ULL << squareIndex(coord);
}

void Board::movePiece(const std::string& from, const std::string& to) {
    if (from.size() != 2 || to.size() != 2) {
        throw std::invalid_argument("movePiece: expected format 'e2e4'");
    }
    int f = squareIndex(from);
    int t = squareIndex(to);
    auto rec = makeMove(f, t);
    // Optionally: could record rec for undo, or print here
     print();
}

// Constants for pawn double-move ranks
static constexpr uint64_t RANK_2 = 0x000000000000FF00ULL;
static constexpr uint64_t RANK_7 = 0x00FF000000000000ULL;

// Move Generation Functions
// Functions were optimized from the original code to use bitboards and precomputed attack tables
// Before I tried to manually check each square, now I use bitwise operations and precomputed masks for efficiency
// This allows for faster move generation and better performance in the chess engine
// Each function generates pseudo-legal moves for the respective piece type, considering the current board state
std::vector<int> Board::generatePawnMoves(int from, bool isWhite) const {
    std::vector<int> moves;
    uint64_t all    = getAllPieces();
    uint64_t own    = isWhite ? getWhitePieces() : getBlackPieces();
    uint64_t opp    = isWhite ? getBlackPieces() : getWhitePieces();
    uint64_t fw     = 1ULL << from;
    uint64_t push;

    if (isWhite) {
        // single push
        push = (fw << 8) & ~all;
        if (push) {
            moves.push_back(__builtin_ctzll(push));
            // double push
            uint64_t push2 = (push << 8) & ~all;
            if ((fw & RANK_2) && push2)
                moves.push_back(__builtin_ctzll(push2));
        }
        // captures
        uint64_t caps = PAWN_ATTACKS_WHITE[from] & opp;
        while (caps) {
            int t = __builtin_ctzll(caps);
            moves.push_back(t);
            caps &= caps - 1;
        }
    } else {
        // single push
        push = (fw >> 8) & ~all;
        if (push) {
            moves.push_back(__builtin_ctzll(push));
            // double push
            uint64_t push2 = (push >> 8) & ~all;
            if ((fw & RANK_7) && push2)
                moves.push_back(__builtin_ctzll(push2));
        }
        // captures
        uint64_t caps = PAWN_ATTACKS_BLACK[from] & opp;
        while (caps) {
            int t = __builtin_ctzll(caps);
            moves.push_back(t);
            caps &= caps - 1;
        }
    }
    return moves;
}
std::vector<int> Board::generateKnightMoves(int from, bool isWhite) const {
    std::vector<int> moves;
    uint64_t mask = KNIGHT_ATTACKS[from] & ~(isWhite ? getWhitePieces() : getBlackPieces());
    while (mask) {
        int t = __builtin_ctzll(mask);
        moves.push_back(t);
        mask &= mask - 1;
    }
    return moves;
}
std::vector<int> Board::generateKingMoves(int from, bool isWhite) const {
    std::vector<int> moves;
    uint64_t own  = isWhite ? getWhitePieces() : getBlackPieces();
    uint64_t mask = KING_ATTACKS[from] & ~own;
    while (mask) {
        int t = __builtin_ctzll(mask);
        moves.push_back(t);
        mask &= mask - 1;
    }
    return moves;
}
std::vector<int> Board::generateRookMoves(int from, bool isWhite) const {
    std::vector<int> moves;
    uint64_t occ = getAllPieces();
    uint64_t own = isWhite ? getWhitePieces() : getBlackPieces();
    uint64_t opp = occ ^ own;
    int dirs[4] = {8, -8, 1, -1};

    for (int dir : dirs) {
        int sq = from + dir;
        while (sq >= 0 && sq < 64 && onSameLine(from, sq, dir)) {
            uint64_t m = 1ULL << sq;
            if (own & m) break;
            moves.push_back(sq);
            if (opp & m) break;
            sq += dir;
        }
    }
    return moves;
}
std::vector<int> Board::generateBishopMoves(int from, bool isWhite) const {
    std::vector<int> moves;
    uint64_t occ = getAllPieces();
    uint64_t own = isWhite ? getWhitePieces() : getBlackPieces();
    uint64_t opp = occ ^ own;
    int dirs[4] = {9, -9, 7, -7};

    for (int dir : dirs) {
        int sq = from + dir;
        while (sq >= 0 && sq < 64 && onSameDiagonal(from, sq, dir)) {
            uint64_t m = 1ULL << sq;
            if (own & m) break;
            moves.push_back(sq);
            if (opp & m) break;
            sq += dir;
        }
    }
    return moves;
}
std::vector<int> Board::generateQueenMoves(int from, bool isWhite) const {
    // Queen moves are rook + bishop
    auto r = generateRookMoves(from, isWhite);
    auto b = generateBishopMoves(from, isWhite);
    r.insert(r.end(), b.begin(), b.end());
    return r;
}

//Generation Helper Functions
bool Board::onSameLine(int f, int t, int d) const {
    if (d == 1 || d == -1)
        return f / 8 == t / 8;
    return true;
}
bool Board::onSameDiagonal(int f, int t, int d) const {
    int df = (t % 8) - (f % 8);
    int dr = (t / 8) - (f / 8);
    return std::abs(df) == std::abs(dr) && ((d == 9 || d == -7) ? df > 0 : df < 0);
}
std::vector<int> Board::generatePseudoLegalMovesForSquare(int sq) const {
    char pc = getPieceAtSquare(sq);
    bool isWhite = (pc >= 'A' && pc <= 'Z');
    switch (pc) {
        case 'P': case 'p': return generatePawnMoves(sq, isWhite);
        case 'N': case 'n': return generateKnightMoves(sq, isWhite);
        case 'B': case 'b': return generateBishopMoves(sq, isWhite);
        case 'R': case 'r': return generateRookMoves(sq, isWhite);
        case 'Q': case 'q': return generateQueenMoves(sq, isWhite);
        case 'K': case 'k': return generateKingMoves(sq, isWhite);
        default: return {};
    }
}


// Test whether square 'sq' is attacked by side 'byWhite'
bool Board::isSquareAttacked(int sq, bool byWhite) const {
    uint64_t occ           = getAllPieces();
    auto      enemyPawns   = byWhite ? whitePawns   : blackPawns;
    auto      enemyKnights = byWhite ? whiteKnights : blackKnights;
    auto      enemyBishops = byWhite ? whiteBishops : blackBishops;
    auto      enemyRooks   = byWhite ? whiteRooks   : blackRooks;
    auto      enemyQueens  = byWhite ? whiteQueens  : blackQueens;
    auto      enemyKing    = byWhite ? whiteKing    : blackKing;

    // Knight attacks
    if (KNIGHT_ATTACKS[sq] & enemyKnights) return true;

    // Pawn attacks — **SWAPPED** so White uses PAWN_ATTACKS_WHITE
    if (byWhite) {
        if (PAWN_ATTACKS_WHITE[sq] & whitePawns) return true;
    } else {
        if (PAWN_ATTACKS_BLACK[sq] & blackPawns) return true;
    }

    // King proximity
    if (KING_ATTACKS[sq] & enemyKing) return true;
    // Sliding attacks: rook-like
    for (int dir : {8, -8, 1, -1}) {
        for (int t = sq + dir; t >= 0 && t < 64 && onSameLine(sq, t, dir); t += dir) {
            uint64_t m = 1ULL << t;
            if (occ & m) {
                if ((enemyRooks | enemyQueens) & m) return true;
                break;
            }
        }
    }

    // Sliding attacks: bishop-like
    for (int dir : {9, -9, 7, -7}) {
        for (int t = sq + dir; t >= 0 && t < 64 && onSameDiagonal(sq, t, dir); t += dir) {
            uint64_t m = 1ULL << t;
            if (occ & m) {
                if ((enemyBishops | enemyQueens) & m) return true;
                break;
            }
        }
    }

    return false;
}


// King-in-check uses square-attacked
bool Board::isKingInCheck(bool isWhite) const {
    uint64_t kingBB = isWhite ? whiteKing : blackKing;
    if (!kingBB) return false;
    int kingSq = __builtin_ctzll(kingBB);
    // “!isWhite” means “is it attacked by the opposite color?”
    return isSquareAttacked(kingSq, !isWhite);
}


// Generate all legal moves by making/unmaking and checking
std::vector<std::pair<int,int>> Board::generateAllLegalMoves(bool isWhite) {
    std::vector<std::pair<int,int>> legal;
    uint64_t pieces = isWhite ? getWhitePieces() : getBlackPieces();

    for (int from = 0; from < 64; ++from) {
        if (!(pieces & (1ULL << from))) continue;
        auto targets = generatePseudoLegalMovesForSquare(from);
        for (int to : targets) {
            Board::MoveRecord rec = const_cast<Board*>(this)->makeMove(from, to);
            if (!isSquareAttacked(__builtin_ctzll(isWhite?whiteKing:blackKing), !isWhite))
                legal.emplace_back(from, to);
            unmakeMove(rec);
        }
    }
    return legal;
}

// Helper to pick bitboard reference by piece char
uint64_t& Board::pieceBitboard(char piece) {
    switch (piece) {
        case 'P': return whitePawns;
        case 'N': return whiteKnights;
        case 'B': return whiteBishops;
        case 'R': return whiteRooks;
        case 'Q': return whiteQueens;
        case 'K': return whiteKing;
        case 'p': return blackPawns;
        case 'n': return blackKnights;
        case 'b': return blackBishops;
        case 'r': return blackRooks;
        case 'q': return blackQueens;
        case 'k': return blackKing;
    }
    static uint64_t dummy = 0;
    return dummy;
}
