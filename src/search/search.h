#pragma once

#include "../game/board/board.h"

int negamax(Board& board, int depth, int alpha, int beta, bool& stop);
Move findBestMove(Board& board, int depth, bool& stop);
int quiescence(Board& board, int alpha, int beta, bool& stop);
int mvvLvaScore(const Move& m);