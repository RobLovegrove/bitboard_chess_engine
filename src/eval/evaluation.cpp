#include "evaluation.h"
#include "pst.h"
#include "pawnStructure.h"


const int pieceValue[12] = {
    100, 500, 320, 330, 900, 20000, // white
    100, 500, 320, 330, 900, 20000  // black
};


int evaluate(Board& board) {

    const uint64_t* bb = board.getAllBitboards();
    int phase = calculateGamePhase(bb);

    int score = material(bb);
    score += pieceSquares(bb, phase);
    score += pawnStructure(bb);

    Colour stm = board.getSideToMove();
    return stm == WHITE ? score : -score;
}

int material(const uint64_t* bb) {
    int score = 0;

    score += __builtin_popcountll(bb[WP]) * pieceValue[WP];
    score += __builtin_popcountll(bb[WN]) * pieceValue[WN];
    score += __builtin_popcountll(bb[WB]) * pieceValue[WB];
    score += __builtin_popcountll(bb[WR]) * pieceValue[WR];
    score += __builtin_popcountll(bb[WQ]) * pieceValue[WQ];

    score -= __builtin_popcountll(bb[BP]) * pieceValue[BP];
    score -= __builtin_popcountll(bb[BN]) * pieceValue[BN];
    score -= __builtin_popcountll(bb[BB]) * pieceValue[BB];
    score -= __builtin_popcountll(bb[BR]) * pieceValue[BR];
    score -= __builtin_popcountll(bb[BQ]) * pieceValue[BQ];

    return score;
}

int calculateGamePhase(const uint64_t* bb) {

    // Calculate game phase
    int phase = __builtin_popcountll(bb[WN]) + __builtin_popcountll(bb[BN]); // knights
    phase += __builtin_popcountll(bb[WB]) + __builtin_popcountll(bb[BB]);    // bishops
    phase += 2 * (__builtin_popcountll(bb[WR]) + __builtin_popcountll(bb[BR])); // rooks
    phase += 4 * (__builtin_popcountll(bb[WQ]) + __builtin_popcountll(bb[BQ])); // queens

    return phase;
}

int pieceSquares(const uint64_t* bb, int phase) {

    // enum PieceType {
    //     WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK,
    //     EMPTY = -1
    // };

    int mgScore = 0;
    int egScore = 0;

    for (int i = WP; i <= WK; i++) {
        uint64_t pieces = bb[i];

        while (pieces) {
            int sq = __builtin_ctzll(pieces);
            pieces &= pieces - 1; 

            int mg = mgTablesWhite[i][sq];
            int eg = egTablesWhite[i][sq];

            mgScore += mg;
            egScore += eg;
        }
    }

    for (int i = BP; i <= BK; i++) {
        uint64_t pieces = bb[i];

        while (pieces) {
            int sq = __builtin_ctzll(pieces);
            pieces &= pieces - 1; 

            int mg = mgTablesBlack[i-6][sq];
            int eg = egTablesBlack[i-6][sq];

            mgScore -= mg;
            egScore -= eg;
        }

    }

    return (mgScore * phase + egScore * (TOTAL_PHASE - phase)) / TOTAL_PHASE; // Normalise at end here

}