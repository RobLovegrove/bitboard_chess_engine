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

    sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return mvvLvaScore(a) > mvvLvaScore(b);  // captures have score 0 for non-captures
    });

    int bestScore = -INF;

    for (Move& m : moves) {
        board.makeMove(m);

        int score = -negamax(board, depth - 1, -beta, -alpha, stop);

        board.unmakeMove(m);

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

    for (Move& m : moves) {

        if (stop) return bestMove;

        board.makeMove(m);

        int score = -negamax(board, depth - 1, -INF, INF, stop);

        board.unmakeMove(m);

        if (score > bestScore || bestMove.isNull()) {
            bestScore = score;
            bestMove = m;
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

    // Keep only captures
    moves.erase(
        remove_if(moves.begin(), moves.end(), [](const Move& m){ return m.capturedPiece == -1; }),
        moves.end()
    );

    // Sort captures by MVV-LVA
    sort(moves.begin(), moves.end(), [](const Move& a, const Move& b){
        return mvvLvaScore(a) > mvvLvaScore(b);
    });

    for (Move& m : moves) {

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