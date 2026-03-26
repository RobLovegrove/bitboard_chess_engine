#pragma once

extern const int mgTablesWhite[6][64];
extern const int egTablesWhite[6][64];
extern int mgTablesBlack[6][64];
extern int egTablesBlack[6][64];

void initPST();
inline int mirror(int sq);