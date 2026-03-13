#pragma once
#include <vector>
#include "../board.h"

std::vector<Move> generatePawnMoves(Board& board, Colour sideToMove);
std::vector<Move> generateKnightMoves(Board& board, Colour sideToMove);
std::vector<Move> generateKingMoves(Board& board, Colour sideToMove);
std::vector<Move> generateBishopMoves(Board& board, Colour sideToMove);
std::vector<Move> generateRookMoves(Board& board, Colour sideToMove);
std::vector<Move> generateQueenMoves(Board& board, Colour sideToMove);
std::vector<Move> generateSlidingMoves(Board& board, 
    Colour sideToMove, 
    int piece, 
    const int directions[], 
    int dirCount);




