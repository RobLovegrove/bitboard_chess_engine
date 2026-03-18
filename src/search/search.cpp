#include "search.h"
#include "../eval/evaluation.h"

using namespace std;

const int INF = 10000000;
const int MATE_SCORE = 100000000;

Move killerMoves[MAX_DEPTH][2] = {};
int history[12][64] = {};

int negamax(Board& board, int depth, int alpha, int beta, int ply, bool& stop) {

    if (stop) return 0;

    if (depth == 0) return quiescence(board, alpha, beta, ply+1, stop);
    //if (depth == 0) return evaluate(board);

    vector<Move> moves = board.generateLegalMoves();

    if (moves.empty()) {
        if (board.isKingInCheck(board.getSideToMove())) return -MATE_SCORE + ply;
        else { return 0; }
    }

    sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        return scoreMove(a, ply) > scoreMove(b, ply);
    });

    int bestScore = -INF;

    for (Move& m : moves) {
        board.makeMove(m);

        int score = -negamax(board, depth - 1, -beta, -alpha, ply+1, stop);

        board.unmakeMove(m);

        if (score > bestScore) bestScore = score;

        if (score > alpha) alpha = score;

        if (score >= beta) {

            if (m.capturedPiece == -1) {
                // shift old killer
                killerMoves[ply][1] = killerMoves[ply][0];
                killerMoves[ply][0] = m;
                history[m.movedPiece][m.to] += depth * depth;
            }
            return beta;
        }

        if (alpha >= beta) break;
    }
    return bestScore;
}

Move findBestMove(Board& board, int depth, bool& stop) {

    // Reset history decay at start of root search
    for (int p = 0; p < 12; ++p) {
        for (int sq = 0; sq < 64; ++sq) {
            history[p][sq] /= 2;
        }
    }

    vector<Move> moves = board.generateLegalMoves();

    if (moves.empty()) { 
        cout << "No legal move found" << endl;
        return Move::null();
     }

    sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b){
        return scoreMove(a, 0) > scoreMove(b, 0);
    });

    Move bestMove = moves[0];
    int bestScore = -INF;

    for (Move& m : moves) {

        if (stop) return bestMove;

        board.makeMove(m);

        int score = -negamax(board, depth - 1, -INF, INF, 0, stop);

        board.unmakeMove(m);

        if (score > bestScore || bestMove.isNull()) {
            bestScore = score;
            bestMove = m;
        }
    }
    return bestMove;
}

int scoreMove(const Move& m, int ply) {

    // 1. Captures → MVV-LVA
    if (m.capturedPiece != -1)
        return 100000 + mvvLvaScore(m);

    // 2. Killer moves
    if (m == killerMoves[ply][0]) return 90000;
    if (m == killerMoves[ply][1]) return 80000;

    // 3. History heuristic
    return history[m.movedPiece][m.to];
}

int quiescence(Board& board, int alpha, int beta, int ply, bool& stop) {

    if (stop) return 0;

    int stand_pat = evaluate(board);

    if (stand_pat >= beta) return beta;

    if (alpha < stand_pat) alpha = stand_pat;

    vector<Move> moves = board.generateLegalMoves();

    // Keep only captures
    moves.erase(
        remove_if(moves.begin(), moves.end(), [](const Move& m){ return m.capturedPiece == -1; }),
        moves.end()
    );

    sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        return mvvLvaScore(a) > mvvLvaScore(b);
    });

    for (Move& m : moves) {

        board.makeMove(m);
        
        int score = -quiescence(board, -beta, -alpha, ply+1, stop);

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