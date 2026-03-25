#pragma once

extern int mgTablesWhite[6][64];
extern int egTablesWhite[6][64];
extern const int mgTablesBlack[6][64];
extern const int egTablesBlack[6][64];

void initPST();
inline int mirror(int sq);