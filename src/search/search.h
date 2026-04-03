#pragma once

#include "../game/board/board.h"
#include "./tt.h"

const int MAX_DEPTH = 64;

extern Move killerMoves[MAX_DEPTH][2];   // 2 killer moves per ply
extern int history[12][64];              // piece × destination square

int negamax(Board& board, 
            TranspositionTable& tt, 
            int depth, int alpha, 
            int beta, int ply, 
            uint64_t& nodes, bool nullAllowed, bool& stop);
Move findBestMove(Board& board, 
                TranspositionTable& tt,
                int depth, Move prevBest, 
                uint64_t& nodes, bool& stop);

int scoreMove(const Move& m, int ply);

int quiescence(Board& board, 
            TranspositionTable& tt, 
            int alpha, int beta, 
            int ply, uint64_t& nodes, 
            bool& stop);
            
int mvvLvaScore(const Move& m);