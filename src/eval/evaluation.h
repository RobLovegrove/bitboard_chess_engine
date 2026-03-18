#pragma once

#include "../game/board/board.h"

static int pieceValue[12] = {
    100, 500, 320, 330, 900, 20000, // white
    100, 500, 320, 330, 900, 20000  // black
};

int evaluate(Board& board);
