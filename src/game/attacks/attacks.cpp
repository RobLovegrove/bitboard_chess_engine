#include "attacks.h"
#include "../board/board.h"

uint64_t knightAttacks[64];
uint64_t kingAttacks[64];
uint64_t pawnAttacks[2][64];

void initAttacks() {
    for (int sq = 0; sq < 64; sq++) {
        int rank = sq / 8;
        int file = sq % 8;

        uint64_t attacks = 0ULL;

        // Knight
        int knightMoves[8][2] = {
            {2,1}, {1,2}, {-1,2}, {-2,1}, {2,-1}, {1,-2}, {-2,-1}, {-1, -2}
        };

        for (auto m : knightMoves) {
            int r = rank + m[0];
            int f = file + m[1];
            if (r >= 0 && r < 8 && f >= 0 && f < 8) {
                attacks |= 1ULL << (r*8 + f); 
                    // Adds a 1 for each square the knight can attack
                    // 1ULL is 000..0001 
                    // r*8 + f converts back to position in bitboard
            }
        }
        knightAttacks[sq] = attacks;

        // King
        attacks = 0ULL;

        int kingMoves[8][2] = {
            {0,1}, {0,-1}, {1,1}, {1,0}, {1,-1}, {-1,-1}, {-1,0}, {-1, 1}
        };

        for (auto m : kingMoves) {
            int r = rank + m[0];
            int f = file + m[1];
            if (r >= 0 && r < 8 && f >= 0 && f < 8) {
                attacks |= 1ULL << (r*8 + f);
            }
        }
        kingAttacks[sq] = attacks;

        // White Pawns
        attacks = 0ULL;
        if (rank < 7) {
            if (file > 0) attacks |= 1ULL << ((rank+1)*8 + file-1);
            if (file < 7) attacks |= 1ULL << ((rank+1)*8 + file+1);
        }

        pawnAttacks[WHITE][sq] = attacks;

        // Black Pawns
        attacks = 0ULL;
        if (rank > 0) {
            if (file > 0) attacks |= 1ULL << ((rank-1)*8 + file-1);
            if (file < 7) attacks |= 1ULL << ((rank-1)*8 + file+1);
        }

        pawnAttacks[BLACK][sq] = attacks;
    }
}