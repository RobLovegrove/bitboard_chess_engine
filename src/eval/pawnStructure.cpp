#include "pawnStructure.h"
#include "../game/board/board.h"

uint64_t fileMasks[8];
uint64_t adjacentFileMasks[8];
uint64_t passedPawnMasks[2][64];

void initPawnMasks() {
    for (int file = 0; file < 8; file++) {
        fileMasks[file] = 0ULL;

        for (int rank = 0; rank < 8; rank++) {
            fileMasks[file] |= (1ULL << (rank * 8 + file));
        }
    }

    for (int file = 0; file < 8; file++) {
        adjacentFileMasks[file] = 0ULL;

        if (file > 0) adjacentFileMasks[file] |= fileMasks[file - 1];
        if (file < 7) adjacentFileMasks[file] |= fileMasks[file + 1];
    }

    for (int sq = 0; sq < 64; sq++) {
        int file = sq % 8;
        int rank = sq / 8;

        passedPawnMasks[WHITE][sq] = 0ULL;
        passedPawnMasks[BLACK][sq] = 0ULL;

        // White looks forward (increasing rank)
        for (int r = rank + 1; r < 8; r++) {
            for (int f = file - 1; f <= file + 1; f++) {
                if (f >= 0 && f < 8) {
                    passedPawnMasks[WHITE][sq] |= (1ULL << (r * 8 + f));
                }
            }
        }

        // Black looks backward (decreasing rank)
        for (int r = rank - 1; r >= 0; r--) {
            for (int f = file - 1; f <= file + 1; f++) {
                if (f >= 0 && f < 8) {
                    passedPawnMasks[BLACK][sq] |= (1ULL << (r * 8 + f));
                }
            }
        }
    }
}

int pawnStructure(const uint64_t* bb) {

    uint64_t whitePawns = bb[WP];
    uint64_t blackPawns = bb[BP];

    int score = 0;

    // White Pawns
    uint64_t pawns = whitePawns;

    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        pawns &= pawns - 1;

        int file = sq % 8;
        int rank = sq / 8;

        // Passed pawns
        if ((passedPawnMasks[WHITE][sq] & blackPawns) == 0) {
            score += rank * 10;
        }

        // Isolated pawns
        if ((adjacentFileMasks[file] & whitePawns) == 0) {
            score -= 15;
        }

        // Doubled pawns
        if (__builtin_popcountll(whitePawns & fileMasks[file]) > 1) {
            score -= 10;
        }
    }

    pawns = blackPawns;
    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        pawns &= pawns - 1;

        int file = sq % 8;
        int rank = sq / 8;

        // Passed pawns
        if ((passedPawnMasks[BLACK][sq] & whitePawns) == 0) {
            score -= (7 - rank) * 10;
        }

        // Isolated pawns
        if ((adjacentFileMasks[file] & blackPawns) == 0) {
            score += 15;
        }

        // Doubled pawns
        if (__builtin_popcountll(blackPawns & fileMasks[file]) > 1) {
            score += 10;
        }
    }

    return score;
}