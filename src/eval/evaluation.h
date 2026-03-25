#pragma once

#include "../game/board/board.h"

constexpr int TOTAL_PHASE = 24;

extern const int pieceValue[12];

int evaluate(Board& board);
int material(const uint64_t* bb);
int calculateGamePhase(const uint64_t* bb);
int pieceSquares(const uint64_t* bb, int phase);
