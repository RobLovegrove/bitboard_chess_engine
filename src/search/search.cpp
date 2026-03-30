#include "search.h"
#include "../eval/evaluation.h"
#include "tt.h"

using namespace std;

const int INF = 10000000;
const int MATE_SCORE = 100000000;

Move killerMoves[MAX_DEPTH][2] = {};
int history[12][64] = {};

int negamax(Board& board, TranspositionTable& tt, 
        int depth, int alpha, int beta, int ply, uint64_t& nodes, bool& stop) {

    if (stop) return 0;
    nodes++; 

    int originalAlpha = alpha;

    // TT probe
    TTEntry* entry = tt.probe(board.getZobristKey());
    if (entry && entry->depth >= depth) {
        if (entry->flag == EXACT)
            return entry->score;
        if (entry->flag == LOWERBOUND)
            alpha = max(alpha, entry->score);
        else if (entry->flag == UPPERBOUND)
            beta = min(beta, entry->score);
        if (alpha >= beta)
            return entry->score;
    }

    if (depth == 0) return quiescence(board, tt, alpha, beta, ply+1, nodes, stop);

    // Null move pruning
    if (depth >= 3 && !board.isKingInCheck(board.getSideToMove())) {
        Undo u;
        board.makeNullMove(u);
        int R = 2; // Depth reduction
        int score = -negamax(
                board, tt, depth - 1 - R, -beta, -beta + 1, ply+1, nodes, stop);
        board.unmakeNullMove(u);

        if (score >= beta) return score; // Fail-soft
    }

    vector<Move> moves = board.generateLegalMoves();

    Move ttMove = Move::null();
    if (entry) {
        ttMove = entry->bestMove;
    }

    if (moves.empty())
        return board.isKingInCheck(board.getSideToMove()) ? -MATE_SCORE + ply : 0;

    // Put best move from TT as first move
    if (!ttMove.isNull()) {
        auto it = find(moves.begin(), moves.end(), ttMove);
        if (it != moves.end()) {
            iter_swap(moves.begin(), it);
        }
    }

    sort(moves.begin() + (ttMove.isNull() ? 0 : 1), moves.end(), [&](const Move& a, const Move& b) {
        return scoreMove(a, ply) > scoreMove(b, ply);
    });

    Move bestMove = Move::null();
    int bestScore = -INF;

    for (Move& m : moves) {

        Undo u;
        board.makeMove(m, u);
        int score = -negamax(board, tt, depth - 1, -beta, -alpha, ply+1, nodes, stop);
        board.unmakeMove(m, u);

        if (score > bestScore) {
            bestScore = score;
            bestMove = m;
        }
        if (score > alpha) alpha = score;
        if (alpha >= beta) {
            if (m.capturedPiece == -1) {
                killerMoves[ply][1] = killerMoves[ply][0];
                killerMoves[ply][0] = m;
                history[m.movedPiece][m.to] += depth * depth;
            }
            tt.store(board.getZobristKey(), depth, bestScore, LOWERBOUND, bestMove);
            return bestScore;  // fail-soft
        }
    }

    NodeType flag = (bestScore <= originalAlpha) ? UPPERBOUND : EXACT;
    tt.store(board.getZobristKey(), depth, bestScore, flag, bestMove);
    return bestScore;
}

Move findBestMove(Board& board, TranspositionTable& tt, 
    int depth, Move prevBest, uint64_t& nodes, bool& stop) {

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


    TTEntry* entry = tt.probe(board.getZobristKey());
    int startIdx = 0;

    // 1. prevBest first
    if (!prevBest.isNull()) {
        auto it = find(moves.begin(), moves.end(), prevBest);
        if (it != moves.end()) {
            iter_swap(moves.begin(), it);
            startIdx = 1;
        }
    }

    // 2. TT move second (if different)
    if (entry) {
        auto it = find(moves.begin() + startIdx, moves.end(), entry->bestMove);
        if (it != moves.end()) {
            iter_swap(moves.begin() + startIdx, it);
            startIdx++;
        }
    }

    sort(moves.begin() + startIdx, moves.end(), [&](const Move& a, const Move& b) {
        return scoreMove(a, 0) > scoreMove(b, 0);
    });

    Move bestMove = moves[0];
    int bestScore = -INF;
    int alpha = -INF;
    int beta = INF;

    for (Move& m : moves) {

        Undo u; 
        if (stop) return bestMove;

        board.makeMove(m, u);

        int score = -negamax(board, tt, depth - 1, -beta, -alpha, 1, nodes, stop);

        board.unmakeMove(m, u);

        if (score > bestScore || bestMove.isNull()) {
            bestScore = score;
            bestMove = m;
        }
        if (score > alpha) alpha = score;
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

int quiescence(Board& board, TranspositionTable& tt, 
    int alpha, int beta, int ply, uint64_t& nodes, bool& stop) {

    if (stop) return 0;

    nodes++;

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

        Undo u;

        board.makeMove(m, u);
        
        int score = -quiescence(board, tt, -beta, -alpha, ply+1, nodes, stop);

        board.unmakeMove(m, u);

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