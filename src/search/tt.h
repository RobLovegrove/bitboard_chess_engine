#pragma once

#include "../game/board/move.h"

#include <cstdint>
#include <vector>

enum NodeType {
    EXACT,
    LOWERBOUND,
    UPPERBOUND
};

struct TTEntry {
    uint64_t key; 
    int depth;
    int score;
    NodeType flag;
    Move bestMove;
};

class TranspositionTable {
public:
    TranspositionTable(size_t size);

    void clear();
    TTEntry* probe(uint64_t key);
    void store(uint64_t key, int depth, int score, NodeType flag, Move bestMove);

private:
    std::vector<TTEntry> table;
    size_t mask;
};