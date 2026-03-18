#include "search.h"
#include "../eval/evaluation.h"

using namespace std;

const int INF = 10000000;
const int MATE_SCORE = 100000000;

int negamax(Board& board, int depth, int alpha, int beta, bool& stop) {

    if (stop) return 0;

    if (depth == 0) return quiescence(board, alpha, beta, stop);
    //if (depth == 0) return evaluate(board);

    vector<Move> moves = board.generateLegalMoves();

    if (moves.empty()) {
        if (board.isKingInCheck(board.getSideToMove())) return -MATE_SCORE; // + ply;  -- Need to add ply count at some point
        else { return 0; }
    }

    sort(moves.begin(), moves.end(), [](Move a, Move b) {
        if (a.capturedPiece != -1 || b.capturedPiece != -1)
            return mvvLvaScore(a) > mvvLvaScore(b);
        return false; // keep original order for non-captures
    });

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

        if (stop) return bestMove;

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

int quiescence(Board& board, int alpha, int beta, bool& stop) {

    if (stop) return 0;

    int stand_pat = evaluate(board);

    if (stand_pat >= beta) return beta;

    if (alpha < stand_pat) alpha = stand_pat;

    vector<Move> moves = board.generateLegalMoves();

    sort(moves.begin(), moves.end(), [](Move a, Move b) {
        if (a.capturedPiece != -1 || b.capturedPiece != -1)
            return mvvLvaScore(a) > mvvLvaScore(b);
        return false; // keep original order for non-captures
    });

    for (Move m : moves) {
        if (m.capturedPiece == -1) continue;

        board.makeMove(m);
        
        int score = -quiescence(board, -beta, -alpha, stop);

        board.unmakeMove(m);

        if (score >= beta) return beta;

        if (score > alpha) alpha = score;
    }

    return alpha;
}

int mvvLvaScore(const Move& m) {
    if (m.capturedPiece == -1) return 0;

    int victim = pieceValue[m.capturedPiece];
    int attacker = pieceValue[m.movedPiece];

    return victim * 10 - attacker;
}