#include "search.h"
#include "../eval/evaluation.h"

using namespace std;

const int INF = 10000000;

int negamax(Board& board, int depth, int alpha, int beta) {

}

Move findBestMove(Board& board, int depth) {

    vector<Move> moves = board.generateLegalMoves();

    Move bestMove;
    int bestScore = -INFINITY;

    for (Move move : moves) {
        board.makeMove(move);

        int score = -negamax(board, --depth, -INF, INF);

        board.unmakeMove(move);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        
    }

    return bestMove;
}