#include "movegen.h"
#include "../attacks/attacks.h"

#include <cstdint>

using namespace std;

vector<Move> generateKnightMoves(Board& board, Colour sideToMove) {

    vector<Move> moves;
    int piece = BN;
    if (sideToMove == WHITE) {
        piece = WN;
    }

    uint64_t knights = board.getBitboard(piece);
    uint64_t ownPieces = board.getPieces(sideToMove);

    while (knights) {
        int sq = __builtin_ctzll(knights);
        knights &= knights - 1;

        uint64_t attacks = knightAttacks[sq] & ~ownPieces;
        while(attacks) {
            int to = __builtin_ctzll(attacks);
            attacks &= attacks - 1;

            // Check if capturing piece
            Move move = Move{sq, to};
            move.capturedPiece = board.getPieceOnSquare(to);
            move.movedPiece = piece;
            moves.push_back(move);
        }
    }
    return moves;
}