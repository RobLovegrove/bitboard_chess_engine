#pragma once
#include <vector>
#include "../board.h"

std::vector<Move> generatePawnMoves(Board& board, Colour sideToMove);
std::vector<Move> generateKnightMoves(Board& board, Colour sideToMove);
std::vector<Move> generateKingMoves(Board& board, Colour sideToMove);

std::vector<Move> generateBishopMoves(Board& board, Colour sideToMove);
std::vector<Move> generateRookMoves(Board& board, Colour sideToMove);
std::vector<Move> generateQueenMoves(Board& board, Colour sideToMove);

uint64_t slidingAttacks(int sq, 
    uint64_t occ, 
    const int directions[], 
    int dirCount);
uint64_t rookAttacks(int square, uint64_t occupancy);
uint64_t bishopAttacks(int square, uint64_t occupancy);
uint64_t bishopAttacks(int square, uint64_t occupancy);




