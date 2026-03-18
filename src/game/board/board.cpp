#include "board.h"
#include "../movegen/movegen.h"
#include "../attacks/attacks.h"

#include <iostream>
#include <sstream>

using namespace std;

const string START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Constructor
Board::Board() {
    init(START_POS);
}

Board::Board(const string fen) {
    init(fen);
}

// Initialise standard chess starting position
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

    ss >> boardPart >> sidePart >> castlingPart >> epPart;

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

    updateOccupancy();    
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
        makeMove(m); // Will swap colour to move at end of makeMove
        if (!isKingInCheck(static_cast<Colour>(sideToMove^1))) {
            legalMoves.push_back(m);
        }
        unmakeMove(m);
    }

    return legalMoves;
}

bool Board::isKingInCheck(Colour stm) {

    int king = (stm == WHITE) ? WK : BK;
    uint64_t kingBB = bitboards[king];
    int kingSq = __builtin_ctzll(kingBB);

    return isSquareAttacked(kingSq, static_cast<Colour>(stm^1));
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
        if (m.capturedPiece != -1) {
            bitboards[m.capturedPiece] &= ~toBB;
        }
        enPassantSquare = -1; // Capturing piece not double pawn push so clear board enpassant flag
    }

    // Handle castling
    if (m.isCastling) {
        if (m.to == 6) { // WK kingside
            bitboards[WR] &= ~(1ULL << 7);
            bitboards[WR] |= (1ULL << 5);
        } else if (m.to == 2) { // WK queenside
            bitboards[WR] &= ~(1ULL << 0);
            bitboards[WR] |= (1ULL << 3);
        } else if (m.to == 62) { // BK kingside
            bitboards[BR] &= ~(1ULL << 63);
            bitboards[BR] |= (1ULL << 61);
        } else if (m.to == 58) { // BK queenside
            bitboards[BR] &= ~(1ULL << 56);
            bitboards[BR] |= (1ULL << 59);
        }
    }

    // Move the piece
    bitboards[m.movedPiece] &= ~fromBB;
    if (m.promotionPiece != -1) {
        bitboards[m.promotionPiece] |= toBB;
    } else {
        bitboards[m.movedPiece] |= toBB;
    }

    // Update castling rights
    if (m.from == 4) castlingRights &= ~(WKS | WQS);
    if (m.from == 60) castlingRights &= ~(BKS | BQS);
    if (m.from == 0 || m.to == 0) castlingRights &= ~WQS;
    if (m.from == 7 || m.to == 7) castlingRights &= ~WKS;
    if (m.from == 56 || m.to == 56) castlingRights &= ~BQS;
    if (m.from == 63 || m.to == 63) castlingRights &= ~BKS;

    updateOccupancy();
    sideToMove = static_cast<Colour>(sideToMove ^ 1);
}

void Board::unmakeMove(Move& m) {

    uint64_t fromBB = 1ULL << m.from;
    uint64_t toBB   = 1ULL << m.to;

    // Remove piece from destination
    bitboards[m.movedPiece] &= ~toBB;

    if (m.promotionPiece != -1) {
        bitboards[m.promotionPiece] &= ~toBB; // Remove promoted piece
        bitboards[m.movedPiece] |= fromBB;   // Restore original pawn
    } else {
        bitboards[m.movedPiece] |= fromBB;
    }

    // Undo castling rook moves
    if (m.isCastling) {
        if (m.to == 6) { // WK kingside
            bitboards[WR] &= ~(1ULL << 5);
            bitboards[WR] |= (1ULL << 7);
        } else if (m.to == 2) { // WK queenside
            bitboards[WR] &= ~(1ULL << 3);
            bitboards[WR] |= (1ULL << 0);
        } else if (m.to == 62) { // BK kingside
            bitboards[BR] &= ~(1ULL << 61);
            bitboards[BR] |= (1ULL << 63);
        } else if (m.to == 58) { // BK queenside
            bitboards[BR] &= ~(1ULL << 59);
            bitboards[BR] |= (1ULL << 56);
        }
    }

    // Restore captured piece
    if (m.isEnpassant) {
        bitboards[m.capturedPiece] |= 1ULL << m.capturedPawnSquare;
    } else if (m.capturedPiece != -1) {
        bitboards[m.capturedPiece] |= toBB;
    }

    // Restore castling rights and en passant square
    castlingRights = m.prevCastlingRights;
    enPassantSquare = m.prevEnpassantSquare;

    updateOccupancy();
    sideToMove = static_cast<Colour>(sideToMove ^ 1);
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
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            cout << ((bb & (1ULL << sq)) ? "1 " : ". ");
        }
        cout << endl;
    }
    cout << endl;
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
    os << squareToString(m.from)
       << squareToString(m.to);

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

        os << promo;
    }

    return os;
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
