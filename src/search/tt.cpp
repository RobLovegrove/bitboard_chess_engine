#include "tt.h"

using namespace std;

TranspositionTable::TranspositionTable(size_t size) {
    table.resize(size);
    mask = size - 1;
}

void TranspositionTable::clear() {
    for (auto& e : table) {
        e.key = 0;
    }
}

TTEntry* TranspositionTable::probe(uint64_t key) {

    TTEntry& entry = table[key & mask];
    if (entry.key == key)
        return &entry;
    
    return nullptr;
}

void TranspositionTable::store(uint64_t key, int depth, int score, NodeType flag, Move bestMove) {

    TTEntry& entry = table[key & mask];

    if (entry.key != key || depth >= entry.depth) {
        entry.key = key;
        entry.depth = depth;
        entry.score = score;
        entry.flag = flag;
        entry.bestMove = bestMove;
    }
}