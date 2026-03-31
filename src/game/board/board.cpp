#include "board.h"
#include "../movegen/movegen.h"
#include "../attacks/attacks.h"
#include "zobrist.h"

#include <iostream>
#include <sstream>
#include <assert.h>

using namespace std;

const string START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Constructor
Board::Board() {
    init(START_POS);
}

Board::Board(const string fen) {
    init(fen);
}

void Board::init(const string fen) {

    // Start Position
    // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

    // Clear all bitboards
    for (int i = 0; i < 12; i++) bitboards[i] = 0ULL;
    whitePieces = blackPieces = occupancy = 0ULL;

    istringstream ss(fen);

    string boardPart;
    string sidePart;
    string castlingPart;
    string epPart;
    string halfmovePart;
    string fullmovePart;

    ss >> boardPart >> sidePart >> castlingPart >> epPart >> halfmovePart >> fullmovePart;

    int rank = 7;
    int file = 0;

    for (char c : boardPart) {
        if (c == '/') {
            rank--;
            file = 0;
            continue;
        } 
        if (isdigit(c)) {
            file += c - '0';
        } else {
            int piece = charToPiece(c);
            if (piece != EMPTY) {
                int sq = rank * 8 + file;
                bitboards[piece] |= 1ULL << sq;
            }
            file++;
        }
    }

    // side to move
    sideToMove = (sidePart == "w") ? WHITE : BLACK;

    // castling rights
    castlingRights = 0;

    if (castlingPart != "-") {
        if (castlingPart.find('K') != std::string::npos) castlingRights |= WKS;
        if (castlingPart.find('Q') != std::string::npos) castlingRights |= WQS;
        if (castlingPart.find('k') != std::string::npos) castlingRights |= BKS;
        if (castlingPart.find('q') != std::string::npos) castlingRights |= BQS;
    }

    // en passant
    if (epPart != "-") {
        int file = epPart[0] - 'a';
        int rank = epPart[1] - '1';
        enPassantSquare = rank * 8 + file;
    }
    else {
        enPassantSquare = -1;
    }

    halfmove = 0;
    fullmove = 1;

    if (ss >> halfmovePart) {
        try { halfmove = stoi(halfmovePart); } catch(...) {}
    }
    if (ss >> fullmovePart) {
        try { fullmove = stoi(fullmovePart); } catch(...) {}
    }

    updateOccupancy();    
    zobristKey = computeZobrist();
}

vector<Move> Board::generatePseudoLegalMoves() {

    vector<Move> moves;

    auto pawnMoves = generatePawnMoves(*this, sideToMove);
    moves.insert(moves.end(), pawnMoves.begin(), pawnMoves.end());

    auto knightMoves = generateKnightMoves(*this, sideToMove);
    moves.insert(moves.end(), knightMoves.begin(), knightMoves.end()); 

    auto kingMoves = generateKingMoves(*this, sideToMove);
    moves.insert(moves.end(), kingMoves.begin(), kingMoves.end()); 

    auto bishopMoves = generateBishopMoves(*this, sideToMove);
    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end()); 

    auto rookMoves = generateRookMoves(*this, sideToMove);
    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end()); 

    auto queenMoves = generateQueenMoves(*this, sideToMove);
    moves.insert(moves.end(), queenMoves.begin(), queenMoves.end()); 
    return moves;
}

std::vector<Move> Board::generateLegalMoves() {

    vector<Move> moves = generatePseudoLegalMoves();
    vector<Move> legalMoves;

    for (Move& m : moves) {
        Undo u;

        preZK = zobristKey;
        uint64_t computedBefore = computeZobrist();

        makeMove(m, u); // Will swap colour to move at end of makeMove
        uint64_t computedAfterMake = computeZobrist();
        uint64_t incrementalAfterMake = zobristKey;

        if (!isKingInCheck(static_cast<Colour>(sideToMove^1))) {
            legalMoves.push_back(m);
        }
        unmakeMove(m, u);
        uint64_t computedAfterUnmake = computeZobrist();

        if (preZK != zobristKey) {
            cout << "Move: " << m << endl;
            cout << "Computed before:       " << hex << computedBefore << endl;
            cout << "Incremental after make: " << incrementalAfterMake << endl;
            cout << "Computed after make:    " << computedAfterMake << endl;
            cout << "Incremental after unmake: " << zobristKey << endl;
            cout << "Computed after unmake:  " << computedAfterUnmake << endl;

            // This tells you which side is broken
            if (incrementalAfterMake != computedAfterMake)
                cout << ">>> makeMove is corrupting the key" << endl;
            else
                cout << ">>> unmakeMove is corrupting the key" << endl;
        }

        assert(preZK == zobristKey);
    }

    return legalMoves;
}

bool Board::isKingInCheck(Colour stm) {

    int king = (stm == WHITE) ? WK : BK;
    uint64_t kingBB = bitboards[king];
    int kingSq = __builtin_ctzll(kingBB);

    return isSquareAttacked(kingSq, static_cast<Colour>(stm^1));
}

void Board::makeMove(Move& m, Undo& u) {

    u.halfmove = halfmove;
    u.fullmove = fullmove;
    u.enPassantSquare = enPassantSquare;
    u.castlingRights = castlingRights;

    u.capturedPiece = m.capturedPiece;
    u.capturedSquare = m.isEnpassant ? m.capturedPawnSquare : m.to;

    if (sideToMove == BLACK) fullmove++;
    halfmove++;
    ply++;

    uint64_t fromBB = 1ULL << m.from;
    uint64_t toBB = 1ULL << m.to;

    // Remove old en passant from zobristKey if it existed and a pawn could capture it
    if (u.enPassantSquare != -1 && pawnCanCaptureEP(u.enPassantSquare, sideToMove))
        zobristKey ^= Zobrist::enpassant[u.enPassantSquare % 8];

    enPassantSquare = -1;

    // Handle captures
    if (m.isEnpassant) {
        bitboards[u.capturedPiece] &= ~(1ULL << u.capturedSquare);
        zobristKey ^= Zobrist::piece[u.capturedPiece][u.capturedSquare];
        halfmove = 0;
    } else if (m.capturedPiece != -1) {
        bitboards[u.capturedPiece] &= ~toBB;
        zobristKey ^= Zobrist::piece[u.capturedPiece][u.capturedSquare];
        halfmove = 0;
    }

    // Set new en passant square
    if (m.epSquareToSet != -1) {
        enPassantSquare = m.epSquareToSet;
        if (pawnCanCaptureEP(enPassantSquare, static_cast<Colour>(sideToMove ^ 1)))
            zobristKey ^= Zobrist::enpassant[enPassantSquare % 8];
    }

    // Handle castling rook
    if (m.isCastling) {
        if (m.to == 6) {
            bitboards[WR] &= ~(1ULL << 7);
            bitboards[WR] |=  (1ULL << 5);
            zobristKey ^= Zobrist::piece[WR][7];
            zobristKey ^= Zobrist::piece[WR][5];
        } else if (m.to == 2) {
            bitboards[WR] &= ~(1ULL << 0);
            bitboards[WR] |=  (1ULL << 3);
            zobristKey ^= Zobrist::piece[WR][0];
            zobristKey ^= Zobrist::piece[WR][3];
        } else if (m.to == 62) {
            bitboards[BR] &= ~(1ULL << 63);
            bitboards[BR] |=  (1ULL << 61);
            zobristKey ^= Zobrist::piece[BR][63];
            zobristKey ^= Zobrist::piece[BR][61];
        } else if (m.to == 58) {
            bitboards[BR] &= ~(1ULL << 56);
            bitboards[BR] |=  (1ULL << 59);
            zobristKey ^= Zobrist::piece[BR][56];
            zobristKey ^= Zobrist::piece[BR][59];
        }
    }

    // Move the piece
    assert(bitboards[m.movedPiece] & fromBB);
    bitboards[m.movedPiece] &= ~fromBB;
    zobristKey ^= Zobrist::piece[m.movedPiece][m.from];

    if (m.promotionPiece != -1) {
        bitboards[m.promotionPiece] |= toBB;
        zobristKey ^= Zobrist::piece[m.promotionPiece][m.to];
    } else {
        bitboards[m.movedPiece] |= toBB;
        zobristKey ^= Zobrist::piece[m.movedPiece][m.to];
    }

    if (m.movedPiece == WP || m.movedPiece == BP)
        halfmove = 0;

    // Update castling rights
    if (m.from == 4)              castlingRights &= ~(WKS | WQS);
    if (m.from == 60)             castlingRights &= ~(BKS | BQS);
    if (m.from == 0 || m.to == 0) castlingRights &= ~WQS;
    if (m.from == 7 || m.to == 7) castlingRights &= ~WKS;
    if (m.from == 56 || m.to == 56) castlingRights &= ~BQS;
    if (m.from == 63 || m.to == 63) castlingRights &= ~BKS;

    zobristKey ^= Zobrist::castling[u.castlingRights];
    zobristKey ^= Zobrist::castling[castlingRights];

    updateOccupancy();
    sideToMove = static_cast<Colour>(sideToMove ^ 1);
    zobristKey ^= Zobrist::side;
}

void Board::unmakeMove(Move& m, Undo& u) {

    zobristKey ^= Zobrist::side;
    sideToMove = static_cast<Colour>(sideToMove ^ 1);

    // Remove current EP from key if it was hashed (sideToMove is now restored)
    if (enPassantSquare != -1 && pawnCanCaptureEP(enPassantSquare, static_cast<Colour>(sideToMove ^ 1)))
        zobristKey ^= Zobrist::enpassant[enPassantSquare % 8];

    halfmove = u.halfmove;
    fullmove = u.fullmove;
    ply--;

    uint64_t fromBB = 1ULL << m.from;
    uint64_t toBB   = 1ULL << m.to;

    // Move piece back
    if (m.promotionPiece != -1) {
        bitboards[m.promotionPiece] &= ~toBB;
        bitboards[m.movedPiece] |= fromBB;
        zobristKey ^= Zobrist::piece[m.promotionPiece][m.to];
        zobristKey ^= Zobrist::piece[m.movedPiece][m.from];
    } else {
        bitboards[m.movedPiece] &= ~toBB;
        bitboards[m.movedPiece] |= fromBB;
        zobristKey ^= Zobrist::piece[m.movedPiece][m.to];
        zobristKey ^= Zobrist::piece[m.movedPiece][m.from];
    }

    // Undo castling rook
    if (m.isCastling) {
        if (m.to == 6) {
            bitboards[WR] &= ~(1ULL << 5);
            bitboards[WR] |=  (1ULL << 7);
            zobristKey ^= Zobrist::piece[WR][5];
            zobristKey ^= Zobrist::piece[WR][7];
        } else if (m.to == 2) {
            bitboards[WR] &= ~(1ULL << 3);
            bitboards[WR] |=  (1ULL << 0);
            zobristKey ^= Zobrist::piece[WR][3];
            zobristKey ^= Zobrist::piece[WR][0];
        } else if (m.to == 62) {
            bitboards[BR] &= ~(1ULL << 61);
            bitboards[BR] |=  (1ULL << 63);
            zobristKey ^= Zobrist::piece[BR][61];
            zobristKey ^= Zobrist::piece[BR][63];
        } else if (m.to == 58) {
            bitboards[BR] &= ~(1ULL << 59);
            bitboards[BR] |=  (1ULL << 56);
            zobristKey ^= Zobrist::piece[BR][59];
            zobristKey ^= Zobrist::piece[BR][56];
        }
    }

    // Restore captured piece
    if (u.capturedPiece != -1) {
        bitboards[u.capturedPiece] |= (1ULL << u.capturedSquare);
        zobristKey ^= Zobrist::piece[u.capturedPiece][u.capturedSquare];
    }

    // Restore castling rights
    zobristKey ^= Zobrist::castling[castlingRights];
    castlingRights = u.castlingRights;
    zobristKey ^= Zobrist::castling[castlingRights];

    // Restore EP square and hash it if a pawn could capture it
    enPassantSquare = u.enPassantSquare;
    if (enPassantSquare != -1 && pawnCanCaptureEP(enPassantSquare, sideToMove))
        zobristKey ^= Zobrist::enpassant[enPassantSquare % 8];

    updateOccupancy();
}

void Board::makeNullMove(Undo& u) {
    u.halfmove = halfmove;
    u.fullmove = fullmove;
    u.enPassantSquare = enPassantSquare;
    u.castlingRights = castlingRights;

    // Remove EP from hash if it exists
    if (enPassantSquare != -1 && pawnCanCaptureEP(enPassantSquare, sideToMove))
        zobristKey ^= Zobrist::enpassant[enPassantSquare % 8];

    enPassantSquare = -1;
    halfmove++;
    ply++;

    sideToMove = static_cast<Colour>(sideToMove ^ 1);
    zobristKey ^= Zobrist::side;
}

void Board::unmakeNullMove(Undo& u) {
    // Restore side first
    sideToMove = static_cast<Colour>(sideToMove ^ 1);
    zobristKey ^= Zobrist::side;

    // Restore EP square and hash if needed
    enPassantSquare = u.enPassantSquare;
    if (enPassantSquare != -1 && pawnCanCaptureEP(enPassantSquare, sideToMove))
        zobristKey ^= Zobrist::enpassant[enPassantSquare % 8];

    halfmove = u.halfmove;
    fullmove = u.fullmove;
    castlingRights = u.castlingRights;
    ply--;
}

bool Board::isLegalMove(Move& move, vector<Move>& moves) {

    for (auto m : moves) {
        if (move.from == m.from && move.to == m.to) {
            if (m.promotionPiece == -1) {
                move = m;
                return true;
            }
            string answer;
            cout << "Would you like to promote to a ";
            cout << static_cast<PieceType>(m.promotionPiece) << ": (y/n) ";
            cin >> answer;
            if (answer == "y" || answer == "yes") {
                move = m;
                return true;
            }
        }
    }
    return false;
}

bool Board::isSquareAttacked(int square, Colour bySide) {

    // Check if any pawns are attacking target square
    uint64_t pawnBB = bitboards[BP];
    uint64_t knightBB = bitboards[BN]; 
    uint64_t kingBB = bitboards[BK];
    uint64_t queenBB = bitboards[BQ];
    uint64_t rookBB = bitboards[BR];
    uint64_t bishopBB = bitboards[BB];
    if (bySide == WHITE) {
        pawnBB = bitboards[WP];
        knightBB = bitboards[WN];
        kingBB = bitboards[WK];
        queenBB = bitboards[WQ];
        rookBB = bitboards[WR];
        bishopBB = bitboards[WB];
    }
    //uint64_t pawnBB = (bySide == WHITE) ? bitboards[WP] : bitboards[BP];

    // Pawn check

    // We want to check what squares white pawns attack our target square
    // So we can use the opposite pawnAttacksMask to go away from target square
    // rather than towards as designed
    if (pawnAttacks[bySide^1][square] & pawnBB) return true;

    // Knight Check
    if (knightAttacks[square] & knightBB) return true;

    // Bishop Check
    if (bishopAttacks(square, occupancy) & (bishopBB | queenBB)) return true;

    // Rook Check
    if (rookAttacks(square, occupancy) & (rookBB | queenBB)) return true;

    // King Check
    if (kingAttacks[square] & kingBB) return true;

    return false;
}

PieceType Board::getPieceOnSquare(int sq) {
    uint64_t mask = 1ULL << sq;

    for (int piece = WP; piece <= BK; piece++) {
        if (bitboards[piece] & mask)
            return static_cast<PieceType>(piece);
    }

    return EMPTY;
}


// Perforamnce Testing
uint64_t Board::perft(int depth) {
    if (depth == 0) return 1;

    uint64_t nodes = 0;
    auto moves = generateLegalMoves();
    for (auto m : moves) {
        Undo u;
        makeMove(m, u);
        assert(zobristKey == computeZobrist());
        nodes += perft(depth - 1);
        unmakeMove(m, u);
        assert(zobristKey == computeZobrist());
    }
    return nodes;
}

// ASCII board printing
void Board::printBoard() const {
    const char* horizontal_border = "  +---+---+---+---+---+---+---+---+";

    for (int rank = 7; rank >=0; rank--) {
        cout << horizontal_border << endl;
        cout << rank+1 << " |";
        for (int file = 0; file < 8; ++file) {
            int sq = rank*8 + file;
            char c = '.';

            for (int p = 0; p < 12; p++) {
                if (bitboards[p] & (1ULL << sq)) {
                    switch(p) {
                        case WP: c='P'; break;
                        case WN: c='N'; break;
                        case WB: c='B'; break;
                        case WR: c='R'; break;
                        case WQ: c='Q'; break;
                        case WK: c='K'; break;
                        case BP: c='p'; break;
                        case BN: c='n'; break;
                        case BB: c='b'; break;
                        case BR: c='r'; break;
                        case BQ: c='q'; break;
                        case BK: c='k'; break;
                    }
                    break;
                }
            }

            cout << " " << c << " |";
        }
        cout << endl;
    }
    cout << horizontal_border << endl;
    cout << "    A   B   C   D   E   F   G   H " << endl << endl;        
}

// ASCII board printing
string Board::printBoardToString() const {

    stringstream os;

    const char* horizontal_border = "  +---+---+---+---+---+---+---+---+";

    for (int rank = 7; rank >=0; rank--) {
        os << horizontal_border << endl;
        os << rank+1 << " |";
        for (int file = 0; file < 8; ++file) {
            int sq = rank*8 + file;
            char c = '.';

            for (int p = 0; p < 12; p++) {
                if (bitboards[p] & (1ULL << sq)) {
                    switch(p) {
                        case WP: c='P'; break;
                        case WN: c='N'; break;
                        case WB: c='B'; break;
                        case WR: c='R'; break;
                        case WQ: c='Q'; break;
                        case WK: c='K'; break;
                        case BP: c='p'; break;
                        case BN: c='n'; break;
                        case BB: c='b'; break;
                        case BR: c='r'; break;
                        case BQ: c='q'; break;
                        case BK: c='k'; break;
                    }
                    break;
                }
            }

            os << " " << c << " |";
        }
        os << endl;
    }
    os << horizontal_border << endl;
    os << "    A   B   C   D   E   F   G   H " << endl << endl;   
    
    return os.str();
}

void Board::printBitboard(uint64_t bb) const {
    for (int rank = 7; rank >= 0; --rank) {
        cout << rank + 1 << " ";
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            cout << ((bb & (1ULL << sq)) ? "1 " : ". ");
        }
        cout << endl;
    }
    cout << "  a b c d e f g h\n\n";
}

// Util Functions
void Board::updateOccupancy() {

    // White Pieces
    whitePieces = bitboards[WP] | bitboards[WR] | bitboards[WN] | bitboards[WB] 
        | bitboards[WQ] | bitboards[WK];

    // Black Pieces
    blackPieces = bitboards[BP] | bitboards[BR] | bitboards[BN] | bitboards[BB] 
        | bitboards[BQ] | bitboards[BK];
    // Occupancy
    occupancy = blackPieces | whitePieces;

}

uint64_t Board::computeZobrist() {

    // Remember ^= is the bitwise XOR
    /* That means:
            If you XOR something in → it’s added
            If you XOR the same thing again → it’s removed
    */

    uint64_t key = 0;

    for (int i = 0; i < 12; i++) {
        uint64_t pieces = bitboards[i];

        while (pieces) {
            int pieceSq = __builtin_ctzll(pieces); 
            key ^= Zobrist::piece[i][pieceSq];
            pieces &= pieces - 1;
        }
    }

    key ^= Zobrist::castling[castlingRights];

    if (enPassantSquare != -1) {
        if (pawnCanCaptureEP(enPassantSquare, sideToMove)) {
            int file = enPassantSquare % 8; // % 8 finds the file of given square 
            key ^= Zobrist::enpassant[file]; 
        }
    }

    if (sideToMove == BLACK) 
        key ^= Zobrist::side;

    return key;
}

bool Board::pawnCanCaptureEP(int epSquare, Colour stm) {
    // Attackers of EP square from the opposite side
    int pawn = (stm == WHITE) ? WP : BP;
    uint64_t attackers = bitboards[pawn] & pawnAttacks[stm^1][epSquare]; // inverted to find pawns that can attack epSquare
    return attackers != 0;
}

int Board::charToPiece(char c) {
    switch (c)
    {
        case 'P': return WP;
        case 'N': return WN;
        case 'B': return WB;
        case 'R': return WR;
        case 'Q': return WQ;
        case 'K': return WK;

        case 'p': return BP;
        case 'n': return BN;
        case 'b': return BB;
        case 'r': return BR;
        case 'q': return BQ;
        case 'k': return BK;
    }

    return EMPTY;
}

// Operator Overeloaders

ostream& operator<<(ostream& os, Colour c) {
    switch(c) {
        case WHITE: os << "White"; break;
        case BLACK: os << "Black"; break;
        default: os << "Unkown"; break;
    }
    return os;
}

ostream& operator<<(ostream& os, PieceType p) {
    switch(p) {
        case WP: os << "White Pawn"; break;
        case WR: os << "White Rook"; break;
        case WN: os << "White Knight"; break;
        case WB: os << "White Bishop"; break;
        case WK: os << "White King"; break;
        case WQ: os << "White Queen"; break;

        case BP: os << "Black Pawn"; break;
        case BR: os << "Black Rook"; break;
        case BN: os << "Black Knight"; break;
        case BB: os << "Black Bishop"; break;
        case BK: os << "Black King"; break;
        case BQ: os << "Black Queen"; break;

        default: os << "Unkown"; break;
    }
    return os;
}

// Convert square index (0-63) to algebraic notation, e.g., 0 -> a1, 63 -> h8
string squareToString(int sq) {
    char file = 'a' + (sq % 8);
    char rank = '1' + (sq / 8);
    return string() + file + rank;
}

// Overload << for Move
ostream& operator<<(ostream& os, const Move& m) {

    os << moveToLAN(m);
    return os;
}

bool operator==(const Move& a, const Move& b) {
    return a.from == b.from &&
           a.to == b.to &&
           a.promotionPiece == b.promotionPiece;
}

bool operator!=(const Move& a, const Move& b) {
    return a.from != b.from ||
           a.to != b.to ||
           a.promotionPiece != b.promotionPiece;
}

string moveToLAN(const Move& m) { // Long algebraic notation

    if (m.isNull()) return "0000";

    string move;

    move = squareToString(m.from) + squareToString(m.to);

    // promotion
    if (m.promotionPiece != -1) {
        char promo;

        switch (m.promotionPiece) {
            case WN: case BN: promo = 'n'; break;
            case WB: case BB: promo = 'b'; break;
            case WR: case BR: promo = 'r'; break;
            case WQ: case BQ: promo = 'q'; break;
            default: promo = 'q';
        }

        move += promo;
    }

    return move;

}

string moveToSAN(const Move& m) {

    stringstream os;
    string san;

    // Castling
    if (m.isCastling) {
        if (m.to > m.from) os << "O-O";       // kingside
        else os << "O-O-O";                   // queenside
        return os.str();
    }

    // Moved piece letter (empty for pawn)
    string pieceChar;
    switch (m.movedPiece) {
        case WN: case BN: pieceChar = "N"; break;
        case WB: case BB: pieceChar = "B"; break;
        case WR: case BR: pieceChar = "R"; break;
        case WQ: case BQ: pieceChar = "Q"; break;
        case WK: case BK: pieceChar = "K"; break;
        default: pieceChar = ""; break; // pawns
    }

    // Capture notation
    string captureChar = "";
    if (m.capturedPiece != -1) {
        captureChar = "x";
        // For pawns, include the file
        if (pieceChar.empty()) {
            char file = 'a' + (m.from % 8);
            pieceChar = string(1, file);
        }
    }

    // Destination square
    string toSquare = squareToString(m.to);

    // Promotion
    string promoChar = "";
    if (m.promotionPiece != -1) {
        switch(m.promotionPiece) {
            case WN: case BN: promoChar = "N"; break;
            case WB: case BB: promoChar = "B"; break;
            case WR: case BR: promoChar = "R"; break;
            case WQ: case BQ: promoChar = "Q"; break;
        }
    }

    os << pieceChar << captureChar << toSquare << promoChar;

    return os.str();
}

string Board::moveToBK(Move& m) {
    if (m.isNull()) return "";

    string s;

    // Castling
    if (m.isCastling) {
        if (m.to > m.from) return "O-O";     // kingside
        else return "O-O-O";                  // queenside
    }

    // Piece letter, empty for pawn
    switch (m.movedPiece) {
        case WN: case BN: s += "N"; break;
        case WB: case BB: s += "B"; break;
        case WR: case BR: s += "R"; break;
        case WQ: case BQ: s += "Q"; break;
        case WK: case BK: s += "K"; break;
        default: break; // pawns
    }

    // Destination square
    int file = m.to % 8;
    int rank = m.to / 8;
    s += char('a' + file);
    s += char('1' + rank);

    // Add '+' if move gives check
    Undo u;
    makeMove(m, u);
    if (isKingInCheck(sideToMove)) s += "+";
    return s;
}
