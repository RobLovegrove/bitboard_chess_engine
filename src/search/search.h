#pragma once

#include "../game/board/board.h"

int negamax(Board& board, int depth, int alpha, int beta, bool& stop);
Move findBestMove(Board& board, int depth, bool& stop);