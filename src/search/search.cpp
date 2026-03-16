#include "search.h"
#include "../eval/evaluation.h"

using namespace std;

const int INF = 10000000;

int negamax(Board& board, int depth, int alpha, int beta) {

    if (depth == 0) return evaluate(board);

    vector<Move> moves = board.generateLegalMoves();

    int bestScore = -INF;

    for (Move move : moves) {
        board.makeMove(move);

        int score = -negamax(board, depth - 1, -beta, -alpha);

        board.unmakeMove(move);

        if (score > bestScore) bestScore = score;

        if (score > alpha) alpha = score;

        if (alpha >= beta) break;
    }
    return bestScore;
}

Move findBestMove(Board& board, int depth) {

    vector<Move> moves = board.generateLegalMoves();

    Move bestMove = moves[0];
    int bestScore = -INF;

    for (Move move : moves) {
        board.makeMove(move);

        int score = -negamax(board, depth - 1, -INF, INF);

        board.unmakeMove(move);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        
    }
    return bestMove;
}