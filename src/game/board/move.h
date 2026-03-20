#pragma once
#include <cstdint>

struct Move {
    int from;
    int to;
    int movedPiece;
    int capturedPiece;
    
    int epSquareToSet = -1;
    int capturedPawnSquare = -1;

    int promotionPiece = -1;

    bool isEnpassant = false;
    bool isCastling = false;

    Move() {}
    
    Move(int f, int t, int movedpiece, int captured, int epSet, int capturePawn, 
        int promoPiece, bool isEP, bool isC)
    : from(f),
        to(t),
        movedPiece(movedpiece),
        capturedPiece(captured),
        epSquareToSet(epSet),
        capturedPawnSquare(capturePawn),
        promotionPiece(promoPiece),
        isEnpassant(isEP),
        isCastling(isC)
    {}

    // Constructor with from/to only
    Move(int f, int t) : Move(f,t,-1, -1, -1,-1,-1, false, false) {}

    static Move null() {
        return Move(-1,-1);
    }

    bool isNull() const {
        return from == -1 && to == -1;
    }
};
