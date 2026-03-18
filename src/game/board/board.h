#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>

enum PieceType {
    WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK,
    EMPTY = -1
};

enum Colour {
    WHITE, BLACK
};

enum CastlingRights {
    WKS = 1,
    WQS = 2,
    BKS = 4,
    BQS = 8 
};

std::ostream& operator<<(std::ostream& os, Colour c);
std::ostream& operator<<(std::ostream& os, PieceType p);

std::string squareToString(int sq);

struct Move {
    int from;
    int to;
    int movedPiece;
    int capturedPiece;

    int prevHalfmove;

    int prevEnpassantSquare = -1;
    int epSquareToSet = -1;
    int capturedPawnSquare = -1;

    int promotionPiece = -1;

    uint8_t prevCastlingRights = 0;

    bool isEnpassant = false;
    bool isCastling = false;

    Move() {}
    
    Move(int f, int t, int movedpiece, int captured, int prevHalfmove, int epSet, int capturePawn, 
        int promoPiece, uint8_t prevCastlingRights, bool isEP, bool isC)
    : from(f),
        to(t),
        movedPiece(movedpiece),
        capturedPiece(captured),
        prevHalfmove(prevHalfmove),
        prevEnpassantSquare(-1),
        epSquareToSet(epSet),
        capturedPawnSquare(capturePawn),
        promotionPiece(promoPiece),
        isEnpassant(isEP),
        isCastling(isC)
    {}

    // Constructor with from/to only
    Move(int f, int t) : Move(f,t,-1, -1, 0, -1,-1,-1, 0, false, false) {}

    static Move null() {
        return Move(-1,-1);
    }

    bool isNull() const {
        return from == -1 && to == -1;
    }
};

std::ostream& operator<<(std::ostream& os, const Move& m);
bool operator==(const Move& a, const Move& b);
std::string moveToSAN(const Move& m);

class Board {
    public:
    Board();
    Board(const std::string FEN_string);

    void printBoard() const;
    std::string printBoardToString() const;
    void printBitboard(uint64_t bb) const;

    void init(const std::string FEN_string);
    std::vector<Move> generatePseudoLegalMoves();
    std::vector<Move> generateLegalMoves();
    void makeMove(Move& m);
    void unmakeMove(Move& m);

    bool isLegalMove(Move& m, std::vector<Move>& moves);
    bool isSquareAttacked(int square, Colour bySide);
    bool isKingInCheck(Colour sideToMove);

    uint64_t perft(int depth);  

    // GETTERS

    uint64_t getBitboard(int piece) const { return bitboards[piece]; }
    uint64_t getOccupancy() const { return occupancy; }
    uint64_t getPieces(Colour sideToMove) const { 
        return (sideToMove == WHITE) ? whitePieces : blackPieces; 
    }
    const uint64_t* getAllBitboards() const { return bitboards; }
    Colour getSideToMove() const { return sideToMove; }
    int getEnPassantSquare() const { return enPassantSquare; }
    uint8_t getCastlingRights() const { return castlingRights; }
    PieceType getPieceOnSquare(int sq);

    // SETTERS
    void setEnPassantSquare(int ePS) {enPassantSquare = ePS; } 

    private:
    uint64_t bitboards[12];               
    uint64_t whitePieces;
    uint64_t blackPieces;
    uint64_t occupancy;

    int halfmove;
    int fullmove;
    int ply;

    void updateOccupancy();
    int charToPiece(char c);
    
    Colour sideToMove;
    int enPassantSquare;
    uint8_t castlingRights;

};

