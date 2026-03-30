#pragma once

#include <cstdint>

extern uint64_t fileMasks[8];
extern uint64_t adjacentFileMasks[8];
extern uint64_t passedPawnMasks[2][64];

void initPawnMasks();

int pawnStructure(const uint64_t* bb);