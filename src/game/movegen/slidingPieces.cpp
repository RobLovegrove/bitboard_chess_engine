#include "movegen.h"

#include <cstdint>

using namespace std;

uint64_t slidingAttacks(int sq, 
    uint64_t occ, 
    const int directions[], 
    int dirCount) {

    uint64_t attacks = 0ULL;

    for (int d = 0; d < dirCount; d++) {
        int toSq = sq + directions[d];
        int prevSq = sq;

        while(toSq >= 0 && toSq < 64) {
            int prevFile = prevSq % 8;
            int toFile = toSq % 8;

            // Prevent wrap around
            if (abs(toFile - prevFile) > 1) break;

            uint64_t toBB = 1ULL << toSq;
            attacks |= toBB;

            // Stop if blocker found
            if (toBB & occ) break; 

            prevSq = toSq;
            toSq += directions[d];
        }
    }

    return attacks;
}


uint64_t rookAttacks(int square, uint64_t occupancy) {
    const int directions[4] = {8, -8, 1, -1};
    return slidingAttacks(square, occupancy, directions, 4);
}
uint64_t bishopAttacks(int square, uint64_t occupancy){
    const int directions[4] = {9, 7, -9, -7};
    return slidingAttacks(square, occupancy, directions, 4);
}
uint64_t queenAttacks(int square, uint64_t occupancy){
    return rookAttacks(square, occupancy) | bishopAttacks(square, occupancy);
}


vector<Move> generateBishopMoves(Board& board, Colour sideToMove) {

    vector<Move> moves;

    uint64_t bishops = board.getBitboard(BB);
    uint64_t occupancy = board.getOccupancy();
    uint64_t ownPieces = board.getPieces(BLACK);
    int piece = BB;

    if (sideToMove == WHITE) {
        piece = WB;
        bishops = board.getBitboard(WB);
        ownPieces = board.getPieces(WHITE);
    }

    while(bishops) {
        int fromSq = __builtin_ctzll(bishops);
        bishops &= bishops - 1;
        
        uint64_t attacks = bishopAttacks(fromSq, occupancy) & ~ownPieces;
        while (attacks) {
            int toSq = __builtin_ctzll(attacks);
            attacks &= attacks - 1;
            
            // Check if capturing piece
            Move move = Move{fromSq, toSq};
            move.movedPiece = piece;
            move.capturedPiece = board.getPieceOnSquare(toSq);
            
            moves.push_back(move);
        }
    }

    return moves;
}

vector<Move> generateRookMoves(Board& board, Colour sideToMove) {

    vector<Move> moves;

    uint64_t rooks = board.getBitboard(BR);
    uint64_t occupancy = board.getOccupancy();
    uint64_t ownPieces = board.getPieces(BLACK);
    int piece = BR;

    if (sideToMove == WHITE) {
        piece = WR;
        rooks = board.getBitboard(WR);
        ownPieces = board.getPieces(WHITE);
    }

    while(rooks) {
        int fromSq = __builtin_ctzll(rooks);
        rooks &= rooks - 1;
        
        uint64_t attacks = rookAttacks(fromSq, occupancy) & ~ownPieces;
        while (attacks) {
            int toSq = __builtin_ctzll(attacks);
            attacks &= attacks - 1;

            // Check if capturing piece
            Move move = Move{fromSq, toSq};
            move.movedPiece = piece;
            move.capturedPiece = board.getPieceOnSquare(toSq);
            
            moves.push_back(move);
        }
    }

    return moves;
}

vector<Move> generateQueenMoves(Board& board, Colour sideToMove) {

    vector<Move> moves;

    uint64_t queens = board.getBitboard(BQ);
    uint64_t occupancy = board.getOccupancy();
    uint64_t ownPieces = board.getPieces(BLACK);
    int piece = BQ;

    if (sideToMove == WHITE) {
        piece = WQ;
        queens = board.getBitboard(WQ);
        ownPieces = board.getPieces(WHITE);
    }

    while(queens) {
        int fromSq = __builtin_ctzll(queens);
        queens &= queens - 1;
        
        uint64_t attacks = queenAttacks(fromSq, occupancy) & ~ownPieces;
        while (attacks) {
            int toSq = __builtin_ctzll(attacks);
            attacks &= attacks - 1;

            // Check if capturing piece
            Move move = Move{fromSq, toSq};
            move.movedPiece = piece;

            move.capturedPiece = board.getPieceOnSquare(toSq);
            
            moves.push_back(move);
        }
    }

    return moves;
}


