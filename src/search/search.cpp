#include "search.h"
#include "../eval/evaluation.h"

using namespace std;

const int INF = 10000000;

int negamax(Board& board, int depth, int alpha, int beta, bool& stop) {

    if (stop) return 0;

    if (depth == 0) return evaluate(board);

    vector<Move> moves = board.generateLegalMoves();

    int bestScore = -INF;

    for (Move move : moves) {
        board.makeMove(move);

        int score = -negamax(board, depth - 1, -beta, -alpha, stop);

        board.unmakeMove(move);

        if (score > bestScore) bestScore = score;

        if (score > alpha) alpha = score;

        if (alpha >= beta) break;
    }
    return bestScore;
}

Move findBestMove(Board& board, int depth, bool& stop) {

    vector<Move> moves = board.generateLegalMoves();

    if (moves.empty()) { 
        cout << "No legal move found" << endl;
        return Move::null();
     }

    Move bestMove = moves[0];
    int bestScore = -INF;

    for (Move move : moves) {

        if (stop) break;

        board.makeMove(move);

        int score = -negamax(board, depth - 1, -INF, INF, stop);

        board.unmakeMove(move);

        if (score > bestScore || bestMove.isNull()) {
            bestScore = score;
            bestMove = move;
        }
    }
    return bestMove;
}