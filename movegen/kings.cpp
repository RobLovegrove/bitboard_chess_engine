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
        Colour opponent = static_cast<Colour>(sideToMove^1);
        if (sideToMove == WHITE) {
            cout << "here" << endl;
            cout << "with caslting rights" << static_cast<int>(board.getCastlingRights()) << endl;
            if (board.getCastlingRights() & WKS) {
                cout << "here" << endl;
                if (!(occ & ((1ULL << 5) | (1ULL << 6)))) {

                    cout << "also here" << endl;
                    // Need to check king is not in check or moving through check
                    if (!board.isSquareAttacked(4, opponent) && 
                        !board.isSquareAttacked(5, opponent) && 
                        !board.isSquareAttacked(6, opponent)) {
                        moves.push_back(Move(4, 6));
                    }
                    else {
                        cout << "square is attacked" << endl;
                    }
                }
            }
            if (board.getCastlingRights() & WQS) {
                if (!(occ & ((1ULL << 3) | (1ULL << 2) | (1ULL << 1)))) {

                    // Need to check king is not in check or moving through check
                    if (!board.isSquareAttacked(4, opponent) && 
                        !board.isSquareAttacked(3, opponent) && 
                        !board.isSquareAttacked(2, opponent)) {
                        moves.push_back(Move(4, 2));
                    }
                    else {
                        cout << "square is attacked" << endl;
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
                        moves.push_back(Move(60, 62));
                    }
                }
            }
            if (board.getCastlingRights() & BQS) {
                if (!(occ & ((1ULL << 59) | (1ULL << 58) | (1ULL << 57)))) {

                    // Need to check king is not in check or moving through check
                    if (!board.isSquareAttacked(60, opponent) && 
                        !board.isSquareAttacked(59, opponent) && 
                        !board.isSquareAttacked(58, opponent)) {
                        moves.push_back(Move(60, 58));
                    }
                }
            }
        }
    }
    return moves;
}