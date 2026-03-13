#include "board.h"
#include "./movegen/movegen.h"
#include "./attacks/attacks.h"

#include <iostream>

using namespace std;

// Constructor
Board::Board() {
    init();
}

// Initialise standard chess starting position
void Board::init() {

    // Clear all bitboards
    for (int i = 0; i < 12; i++) bitboards[i] = 0ULL;
    whitePieces = blackPieces = occupancy = 0ULL;

    // Pawns
    bitboards[WP] = 0x000000000000FF00ULL; // Hexidecimal with LSB = A1 on board 
    bitboards[BP] = 0x00FF000000000000ULL;

    // Rooks
    bitboards[WR] = 0x0000000000000081ULL;
    bitboards[BR] = 0x8100000000000000ULL;

    // Knights
    bitboards[WN] = 0x0000000000000042ULL;
    bitboards[BN] = 0x4200000000000000ULL;

    // Bishops
    bitboards[WB] = 0x0000000000000024ULL;
    bitboards[BB] = 0x2400000000000000ULL;

    // Queens
    bitboards[WQ] = 0x0000000000000008ULL;
    bitboards[BQ] = 0x0800000000000000ULL;

    // Kings
    bitboards[WK] = 0x0000000000000010ULL;
    bitboards[BK] = 0x1000000000000000ULL;

    updateOccupancy();

    sideToMove = WHITE;
    enPassantSquare = -1;

    castlingRights = WKS | WQS | BKS | BQS;
}

vector<Move> Board::generateMoves() {

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

void Board::makeMove(Move& m) {

    uint64_t fromBB = 1ULL << m.from;
    uint64_t toBB = 1ULL << m.to;
    m.prevEnpassantSquare = enPassantSquare;
    m.prevCastlingRights = castlingRights;

    if (m.isEnpassant) {
        // Pawn is being captured by enpassant
        bitboards[m.capturedPiece] &= ~(1ULL << m.capturedPawnSquare);
        enPassantSquare = -1; 
    }
    else if (m.epSquareToSet != -1) {
        // Double pawn push so setting board.enpassantSquare
        enPassantSquare = m.epSquareToSet;

    }
    else {
        // Remove captured piece
        for (int i = 0; i < PIECE_NB; i++) {
            if (bitboards[i] & toBB) {
                m.capturedPiece = i;
                bitboards[i] &= ~toBB;
                break;
            }
        }
        enPassantSquare = -1; // Capturing piece not double pawn push so clear board enpassant flag
    }

    // Move piece
    for (int i = 0; i < PIECE_NB; i++) {

        if (i == WK && m.from == 4 && m.to == 6) {
            // Castling so move rook
            bitboards[WR] &= ~(1ULL << 7);
            bitboards[WR] |= (1ULL << 5);
        }
        else if (i == WK && m.from == 4 && m.to == 2) {
            // Castling so move rook
            bitboards[WR] &= ~(1ULL << 0);
            bitboards[WR] |= (1ULL << 3);
        }
        else if (i == BK && m.from == 60 && m.to == 62) {
            // Castling so move rook
            bitboards[BR] &= ~(1ULL << 63);
            bitboards[BR] |= (1ULL << 61);
        }
        else if (i == BK && m.from == 60 && m.to == 58) {
            // Castling so move rook
            bitboards[BR] &= ~(1ULL << 56);
            bitboards[BR] |= (1ULL << 59);
        }

        if (bitboards[i] & fromBB) {
            bitboards[i] &= ~fromBB;
            if (m.promotionPiece != -1) bitboards[m.promotionPiece] |= toBB;
            else { bitboards[i] |= toBB; }
            break;
        }
    }

    if (m.from == 4) castlingRights &= ~(WKS | WQS);
    if (m.from == 60) castlingRights &= ~(BKS | BQS);

    if (m.from == 0 || m.to == 0) castlingRights &= ~WQS;
    if (m.from == 7 || m.to == 7) castlingRights &= ~WKS;
    if (m.from == 56 || m.to == 56) castlingRights &= ~BQS;
    if (m.from == 63 || m.to == 63) castlingRights &= ~BKS;

    updateOccupancy();
    sideToMove = static_cast<Colour>(sideToMove ^ 1); // Rasiing to power 1 inverts
}

void Board::unmakeMove(Move& m) {

    uint64_t fromBB = 1ULL << m.from;
    uint64_t toBB = 1ULL << m.to;

    // Move piece back
    for (int i = 0; i < PIECE_NB; i++) {
        if (bitboards[i] & toBB) {
            if (m.promotionPiece != -1) bitboards[m.promotionPiece] &= ~toBB;
            else { bitboards[i] &= ~toBB; }
            bitboards[i] |=fromBB;
            break;
        }
    }

    // Undo setting EnPassantSquare
    enPassantSquare = m.prevEnpassantSquare;

    // Undo Castling rights change
    castlingRights = m.prevCastlingRights;

    if (m.isEnpassant) {
        // Restore enpassant captured pawn
        bitboards[m.capturedPiece] |= 1ULL << m.capturedPawnSquare;
    }
    // Restore captured piece
    else if (m.capturedPiece != -1) {
        bitboards[m.capturedPiece] |= toBB;
    }

    updateOccupancy();
    sideToMove = static_cast<Colour>(sideToMove ^ 1);;

}

bool Board::isLegalMove(Move& move) {
    auto moves = generateMoves();

    for (auto m : moves) {
        if (move.from == m.from && move.to == m.to) {
            if (m.promotionPiece == -1) {
                move = m;
                return true;
            }
            string answer;
            cout << "Would you like to promote to a ";
            cout << static_cast<PieceType>(m.promotionPiece) << ": (y/n)";
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


// Perforamnce Testing
uint64_t Board::perft(int depth) {
    if (depth == 0) return 1;

    uint64_t nodes = 0;

    auto moves = generateMoves();

    for (auto m : moves) {
        makeMove(m);
        nodes += perft(depth - 1);
        unmakeMove(m);
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

            for (int p = 1; p <= 12; p++) {
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

void Board::printBitboard(uint64_t bb) const {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            cout << ((bb & (1ULL << sq)) ? "1 " : ". ");
        }
        cout << std::endl;
    }
    cout << std::endl;
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
