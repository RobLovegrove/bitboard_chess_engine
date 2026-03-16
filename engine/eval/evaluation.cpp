#include "evaluation.h"

int evaluate(Board& board) {

    int score = 0;

    const uint64_t* bitboards = board.getAllBitboards();

    score += __builtin_popcountll(bitboards[WP]) * 100;
    score += __builtin_popcountll(bitboards[WN]) * 320;
    score += __builtin_popcountll(bitboards[WB]) * 330;
    score += __builtin_popcountll(bitboards[WR]) * 500;
    score += __builtin_popcountll(bitboards[WQ]) * 900;

    score -= __builtin_popcountll(bitboards[BP]) * 100;
    score -= __builtin_popcountll(bitboards[BN]) * 320;
    score -= __builtin_popcountll(bitboards[BB]) * 330;
    score -= __builtin_popcountll(bitboards[BR]) * 500;
    score -= __builtin_popcountll(bitboards[BQ]) * 900;

    Colour stm = board.getSideToMove();
    return stm == WHITE ? score : -score;
}