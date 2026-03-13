#include "movegen.h"
#include "../attacks/attacks.h"
#include"../board.h"

#include <cstdint>
#include <iostream> 
using namespace std;

// Pawn single push
vector<Move> generatePawnMoves(Board& board, Colour sideToMove) {
    vector<Move> moves;
    int piece = BP;
    uint64_t startingRank = 0x00FF000000000000ULL; // Rank 1
    uint64_t promoRank = 0x00000000000000FFULL;
    int blackPromoPieces[4] = {BQ, BR, BN, BB};
    int whitePromoPieces[4] = {WQ, WR, WN, WB}; 
    int* promoPieces = blackPromoPieces;

    if (sideToMove == WHITE) {
        piece = WP;
        startingRank = 0x00000000000000FF00ULL;
        promoRank = 0xFF00000000000000ULL; // Rank 8
        promoPieces = whitePromoPieces;
    }
    uint64_t pawns = board.getBitboard(piece);
    uint64_t empty = ~board.getOccupancy();
    uint64_t opponentPieces = board.getPieces(static_cast<Colour>(sideToMove^1)); // Power of 1 inverts side

    // Single pawn push
    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        uint64_t fromBB = 1ULL << sq;
        uint64_t oneStep;
        if (sideToMove == WHITE) oneStep = fromBB << 8;
        else { oneStep = fromBB >> 8; }
        
        if (oneStep & empty) {
            int toSq = (sideToMove == WHITE) ? sq + 8 : sq - 8;
            Move move = Move{sq, toSq};

            // Promotion
            if (oneStep & promoRank) {
                for (int i = 0; i < 4; i++) {
                    move.promotionPiece = promoPieces[i];
                    moves.push_back(move);
                }
            }
            else { moves.push_back(move); }

            // Double pawn push
            if (fromBB & startingRank) {
                uint64_t twoStep;
                if (sideToMove == WHITE) twoStep = oneStep << 8;
                else { twoStep = oneStep >> 8; }

                if (twoStep & empty) {
                    int eps = toSq;
                    toSq = (sideToMove == WHITE) ? sq + 16 : sq - 16;
                    moves.push_back(Move{sq, toSq, -1, eps, -1, -1, 0, false});
                }
            }
        }

        // Pawn attacks
        uint64_t attacks = pawnAttacks[sideToMove][sq];
        uint64_t normalCaptures = attacks & opponentPieces;
            
        while(normalCaptures) {
            int toSq = __builtin_ctzll(normalCaptures);
            normalCaptures &= normalCaptures - 1;
            Move move = Move{sq, toSq};

            // Promotion
            if ((1ULL << toSq) & promoRank) {
                for (int i = 0; i < 4; i++) {
                    move.promotionPiece = promoPieces[i];
                    moves.push_back(move);
                }
            }
            else { moves.push_back(Move{sq, toSq}); }
        }

        int eps = board.getEnPassantSquare();
        if (eps != -1) {
            if (attacks & (1ULL << eps)) {
                int capturedPawn = WP;
                int pawnSq = eps + 8;
                if (sideToMove == WHITE) {
                    capturedPawn = BP;
                    pawnSq = eps - 8;
                }
                moves.push_back(Move(sq, eps, capturedPawn, -1, pawnSq, -1, 0, true));
            }
        }

        pawns &= pawns - 1;
    }
    return moves;
}