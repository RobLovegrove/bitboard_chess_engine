#include "movegen.h"
#include "../attacks/attacks.h"

#include <cstdint>

using namespace std;

vector<Move> generateKingMoves(Board& board, Colour sideToMove) {

    vector<Move> moves;
    int piece = BK;
    if (sideToMove == WHITE) {
        piece = WK;
    }

    uint64_t kings = board.getBitboard(piece);
    uint64_t ownPieces = board.getPieces(sideToMove);
    uint64_t occ = board.getOccupancy();

    while (kings) {
        int sq = __builtin_ctzll(kings);
        kings &= kings - 1;

        // Normal King moves
        uint64_t attacks = kingAttacks[sq] & ~ownPieces;
        while(attacks) {
            int to = __builtin_ctzll(attacks);
            attacks &= attacks - 1;
            moves.push_back(Move{sq, to});
        }

        // Castling
        if (sideToMove == WHITE) {
            if (board.getCastlingRights() & WKS) {
                if (!(occ & ((1ULL << 5) | (1ULL << 6)))) {

                    // TODO: Need to check king is not in check or moving through check

                    moves.push_back(Move(4, 6));

                }
            }
            if (board.getCastlingRights() & WQS) {
                if (!(occ & ((1ULL << 3) | (1ULL << 2) | (1ULL << 1)))) {

                    // TODO: Need to check king is not in check or moving through check

                    moves.push_back(Move(4, 2));
                }
            }
        }
        if (sideToMove == BLACK) {
            if (board.getCastlingRights() & BKS) {
                if (!(occ & ((1ULL << 61) | (1ULL << 62)))) {

                    // TODO: Need to check king is not in check or moving through check

                    moves.push_back(Move(60, 62));

                }
            }
            if (board.getCastlingRights() & BQS) {
                if (!(occ & ((1ULL << 59) | (1ULL << 58) | (1ULL << 57)))) {

                    // TODO: Need to check king is not in check or moving through check

                    moves.push_back(Move(60, 58));
                }
            }
        }
    }
    return moves;
}