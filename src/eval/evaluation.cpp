#include "evaluation.h"


const int pieceValue[12] = {
    100, 500, 320, 330, 900, 20000, // white
    100, 500, 320, 330, 900, 20000  // black
};

const int pawnTableMG[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10,-20,-20, 10, 10,  5,
     5, -5,-10,  0,  0,-10, -5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5,  5, 10, 25, 25, 10,  5,  5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
     0,  0,  0,  0,  0,  0,  0,  0
};

const int pawnTableEG[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    60, 60, 60, 60, 60, 60, 60, 60,
    50, 50, 50, 50, 50, 50, 50, 50,
    40, 40, 40, 40, 40, 40, 40, 40,
    30, 30, 30, 30, 30, 30, 30, 30,
    20, 20, 20, 20, 20, 20, 20, 20,
    10, 10, 10, 10, 10, 10, 10, 10,
     0,  0,  0,  0,  0,  0,  0,  0
};

const int knightTable[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

const int bishopTable[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

const int rookTableMG[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};

const int rookTableEG[64] = {
     0,   0,   5,  10,  10,   5,   0,   0,
    25,  25,  25,  25,  25,  25,  25,  25,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,   0,   5,   5,   0,   0,   0
};

const int queenTable[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

const int kingTableMG[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

const int kingTableEG[64] = {
     0,  5, 10, 15, 15, 10,  5,  0,
     5, 10, 15, 20, 20, 15, 10,  5,
    10, 15, 20, 25, 25, 20, 15, 10,
    15, 20, 25, 30, 30, 25, 20, 15,
    15, 20, 25, 30, 30, 25, 20, 15,
    10, 15, 20, 25, 25, 20, 15, 10,
     5, 10, 15, 20, 20, 15, 10,  5,
     0,  5, 10, 15, 15, 10,  5,  0
};

const int zeroTable[64] = {0};

const int* mgTables[6] = { pawnTableMG, rookTableMG, knightTable, bishopTable, queenTable, kingTableMG };
const int* egTables[6] = { pawnTableEG, rookTableEG, zeroTable, zeroTable, zeroTable, kingTableEG };

int evaluate(Board& board) {

    const uint64_t* bb = board.getAllBitboards();

    int score = material(bb);
    float phase = calculateGamePhase(bb);
    score += pieceSquares(bb, phase);

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

int mirror(int sq) { return sq ^ 56; } // flips rank of pieceTables

int calculateGamePhase(const uint64_t* bb) {

    // Calculate game phase
    int phase = __builtin_popcountll(bb[WN]) + __builtin_popcountll(bb[BN]); // knights
    phase += __builtin_popcountll(bb[WB]) + __builtin_popcountll(bb[BB]);    // bishops
    phase += 2 * (__builtin_popcountll(bb[WR]) + __builtin_popcountll(bb[BR])); // rooks
    phase += 4 * (__builtin_popcountll(bb[WQ]) + __builtin_popcountll(bb[BQ])); // queens

    float gamePhase = phase / (float)TOTAL_PHASE;  // normalize 0..1

    return gamePhase;
}

int pieceSquares(const uint64_t* bb, float gamePhase) {

    int score = 0;

    // enum PieceType {
    //     WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK,
    //     EMPTY = -1
    // };

    for (int i = 0; i < 12; i++) {
        uint64_t pieces = bb[i];
        bool isWhite = i < 6;
        int type = i % 6; // 0 = P, 1 = R, 2 = N, 3 = B, 4 = Q, 5 = K

        while(pieces) {
            int sq = __builtin_ctzll(pieces);
            pieces &= pieces - 1;     
            
            int tableScore = isWhite ? mgTables[type][sq]*gamePhase + egTables[type][sq]*(1-gamePhase)
                     : mgTables[type][mirror(sq)]*gamePhase + egTables[type][mirror(sq)]*(1-gamePhase);

            score += isWhite ? tableScore : -tableScore;
        }

    }

    return score;
}