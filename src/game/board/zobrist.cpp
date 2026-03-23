#include "zobrist.h"

#include <random>

namespace {
    uint64_t rand64() { 
        static std::mt19937_64 rng(123456); // Fixed seed
        return rng();
    }
}

uint64_t Zobrist::piece[12][64];
uint64_t Zobrist::castling[16];
uint64_t Zobrist::enpassant[8];
uint64_t Zobrist::side;

void Zobrist::init() {
    for (int p = 0; p < 12; p++)
        for (int sq = 0; sq < 64; ++sq)
            piece[p][sq] = rand64();

    for (int i = 0; i < 16; i++)
        castling[i] = rand64();

    for (int i = 0; i < 8; i++)
        enpassant[i] = rand64();

    side = rand64();
}
