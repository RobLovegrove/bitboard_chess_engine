#include "movegen.h"

#include <cstdint>

using namespace std;

vector<Move> generateSlidingMoves(Board& board, 
    Colour sideToMove, 
    int piece, 
    const int directions[], 
    int dirCount) {

    vector<Move> moves;

    uint64_t pieces = board.getBitboard(piece);
    uint64_t ownPieces = board.getPieces(static_cast<Colour>(sideToMove));
    uint64_t opponentPieces = board.getPieces(static_cast<Colour>(sideToMove^1));

    while (pieces) {
        int fromSq = __builtin_ctzll(pieces);
        pieces &= pieces - 1;

        for (int d = 0; d < dirCount ; d++) {
            int toSq = fromSq + directions[d];
            int prevSq = fromSq;
            while (toSq >= 0 && toSq < 64) {
                int prevFile = prevSq % 8;
                int toFile   = toSq % 8;

                if (abs(toFile - prevFile) > 1) break;
                
                uint64_t toBB = 1ULL << toSq;
                if (toBB & ownPieces) break;
                moves.push_back(Move(fromSq, toSq));
                if (toBB & opponentPieces) break;

                prevSq = toSq;
                toSq += directions[d];
            }   
        }
    }
    return moves;
}



vector<Move> generateBishopMoves(Board& board, Colour sideToMove) {

    int piece = BB;
    if (sideToMove == WHITE) {
        piece = WB;
    }

    int directions[4] = {9, 7, -9, -7};

    vector<Move> moves = generateSlidingMoves(board, sideToMove, piece, directions, 4);
    return moves;
}

vector<Move> generateRookMoves(Board& board, Colour sideToMove) {

    int piece = BR;
    if (sideToMove == WHITE) {
        piece = WR;
    }
    int directions[4] = {8, -8, 1, -1};
    vector<Move> moves = generateSlidingMoves(board, sideToMove, piece, directions, 4);
    return moves;
}

vector<Move> generateQueenMoves(Board& board, Colour sideToMove) {

    int piece = BQ;
    if (sideToMove == WHITE) {
        piece = WQ;
    }
    int directions[8] = {8, -8, 1, -1, 9, 7, -9, -7};
    vector<Move> moves = generateSlidingMoves(board, sideToMove, piece, directions, 8);
    return moves;
}


