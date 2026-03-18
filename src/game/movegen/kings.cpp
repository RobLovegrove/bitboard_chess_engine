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

            // Check if capturing piece
            Move move = Move{sq, to};
            move.capturedPiece = board.getPieceOnSquare(to);
            move.movedPiece = piece;
            moves.push_back(move);
        }

        // Castling
        Colour opponent = static_cast<Colour>(sideToMove^1);
        if (sideToMove == WHITE) {
            if (board.getCastlingRights() & WKS) {
                if (!(occ & ((1ULL << 5) | (1ULL << 6)))) {
                    // Need to check king is not in check or moving through check
                    if (!board.isSquareAttacked(4, opponent) && 
                        !board.isSquareAttacked(5, opponent) && 
                        !board.isSquareAttacked(6, opponent)) {
                            Move m = Move(4,6);
                            m.isCastling = true;
                            m.movedPiece = piece;
                            moves.push_back(m);
                    }
                }
            }
            if (board.getCastlingRights() & WQS) {
                if (!(occ & ((1ULL << 3) | (1ULL << 2) | (1ULL << 1)))) {

                    // Need to check king is not in check or moving through check
                    if (!board.isSquareAttacked(4, opponent) && 
                        !board.isSquareAttacked(3, opponent) && 
                        !board.isSquareAttacked(2, opponent)) {
                            Move m = Move(4,2);
                            m.isCastling = true;
                            m.movedPiece = piece;
                            moves.push_back(m);
                    }
                }
            }
        }
        if (sideToMove == BLACK) {
            if (board.getCastlingRights() & BKS) {
                if (!(occ & ((1ULL << 61) | (1ULL << 62)))) {

                    // Need to check king is not in check or moving through check
                    if (!board.isSquareAttacked(60, opponent) && 
                        !board.isSquareAttacked(61, opponent) && 
                        !board.isSquareAttacked(62, opponent)) {
                            Move m = Move(60,62);
                            m.isCastling = true;
                            m.movedPiece = piece;
                            moves.push_back(m);
                    }
                }
            }
            if (board.getCastlingRights() & BQS) {
                if (!(occ & ((1ULL << 59) | (1ULL << 58) | (1ULL << 57)))) {

                    // Need to check king is not in check or moving through check
                    if (!board.isSquareAttacked(60, opponent) && 
                        !board.isSquareAttacked(59, opponent) && 
                        !board.isSquareAttacked(58, opponent)) {
                            Move m = Move(60,58);
                            m.isCastling = true;
                            m.movedPiece = piece;
                            moves.push_back(m);
                    }
                }
            }
        }
    }
    return moves;
}