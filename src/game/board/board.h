#pragma once

#include "move.h"

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

struct Undo {
    int capturedPiece;
    int capturedSquare;
    int castlingRights;
    int enPassantSquare;
    int halfmove;
    int fullmove;
};

std::ostream& operator<<(std::ostream& os, const Move& m);
bool operator==(const Move& a, const Move& b);
std::string moveToSAN(const Move& m);
std::string moveToLAN(const Move& m);

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
    void makeMove(Move& m, Undo& u);
    void unmakeMove(Move& m, Undo& u);

    void makeNullMove(Undo& u); // For null move pruning
    void unmakeNullMove(Undo& u);

    bool isLegalMove(Move& m, std::vector<Move>& moves);
    bool isSquareAttacked(int square, Colour bySide);
    bool isKingInCheck(Colour sideToMove);
    bool moveGivesCheck(Move move);

    uint64_t perft(int depth);  

    std::string moveToBK(Move& m); // Needed for comparing bm in positions.txt

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
    uint64_t getZobristKey() const { return zobristKey; }

    uint64_t computeZobrist();

    // SETTERS
    void setEnPassantSquare(int ePS) {enPassantSquare = ePS; } 

    private:
    uint64_t bitboards[12];               
    uint64_t whitePieces;
    uint64_t blackPieces;
    uint64_t occupancy;

    uint64_t zobristKey;
    uint64_t preZK; // Testing purposes only

    int halfmove;
    int fullmove;
    int ply;

    void updateOccupancy();
    int charToPiece(char c);

    bool pawnCanCaptureEP(int epSquare, Colour side);

    Colour sideToMove;
    int enPassantSquare;
    uint8_t castlingRights;

};

