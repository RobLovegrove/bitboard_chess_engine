#pragma once

#include "../game/board/board.h"

constexpr int TOTAL_PHASE = 24;

extern const int pieceValue[12];
extern const int pawnTableMG[64];
extern const int pawnTableEG[64];
extern const int knightTable[64];
extern const int bishopTable[64];
extern const int rookTableMG[64];
extern const int rookTableEG[64];
extern const int queenTable[64];
extern const int kingTableMG[64];
extern const int kingTableEG[64];
extern const int zeroTable[64];

extern const int* mgTables[6];
extern const int* egTables[6];

int mirror(int sq);

int evaluate(Board& board);
int material(const uint64_t* bb);
int calculateGamePhase(const uint64_t* bb);
int pieceSquares(const uint64_t* bb, float gamePhase);
