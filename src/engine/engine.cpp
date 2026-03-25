#include "./engine.h"
#include "../search/search.h"
#include "../game/attacks/attacks.h"
#include "../game/board/zobrist.h"
#include "../search/tt.h"
#include "../eval/pst.h"

#include <sstream>

using namespace std;

Engine::Engine() : tt(1 << 20) {

    // Initilaise the attack masks
    initAttacks();

    // Initilaise the zobrist hasing
    Zobrist::init();

    // Initilaise the piece square tables for evaluator
    initPST();

    // Initilalise the board
    board = Board(startPos);
}

void Engine::newGame() {
    board = Board(startPos);
    tt.clear();
}

void Engine::newGameWithFEN(const string& fen) {
    board = Board(fen);
    tt.clear();
}

int Engine::charToPiece(char c) {
    switch (tolower(c)) {
        case 'q': return (board.getSideToMove() == WHITE) ? WQ : BQ;
        case 'r': return (board.getSideToMove() == WHITE) ? WR : BR;
        case 'b': return (board.getSideToMove() == WHITE) ? WB : BB;
        case 'n': return (board.getSideToMove() == WHITE) ? WN : BN;
        default: return EMPTY; // invalid promotion
    }
}

bool Engine::parseMove(
    const std::string& input, int& from, int& to, int& promo) {
    promo = EMPTY;

    if (input.length() < 4 || input.length() > 5) return false;

    // From square
    if (input[0] < 'a' || input[0] > 'h') return false;
    if (input[1] < '1' || input[1] > '8') return false;
    from = (input[1] - '1') * 8 + (input[0] - 'a');

    // To square
    if (input[2] < 'a' || input[2] > 'h') return false;
    if (input[3] < '1' || input[3] > '8') return false;
    to = (input[3] - '1') * 8 + (input[2] - 'a');

    // Optional promotion
    if (input.length() == 5) {
        promo = charToPiece(input[4]);
        if (promo == EMPTY) return false;
    }

    return true;
}

bool Engine::isGameOver(string& result) {
    vector<Move> legalMoves = board.generateLegalMoves();

    if (legalMoves.empty()) {
        if (board.isKingInCheck(board.getSideToMove())) {
            result = "CHECKMATE";
            return true;
        }
        else {
            result = "STALEMATE";
            return true;
        }
    }
    return false;
}

void Engine::makeMove(const string& moveStr) {

    vector<Move> legalMoves = board.generateLegalMoves();
    Move move;

    if (!parseMove(moveStr, move.from, move.to, move.promotionPiece)) {
        throw runtime_error("Invalid input: " + moveStr);
    }

    for (auto& m : legalMoves) {
        if (m.from == move.from && m.to == move.to) {
            m.promotionPiece = move.promotionPiece;
            Undo u;
            board.makeMove(m, u);
            return;
        }
    }

    string colour = (board.getSideToMove() == WHITE) ? "WHITE" : "BLACK";
    throw runtime_error("Illegal move by " + colour + ": " + moveStr);
}

void Engine::makeMove(Move& move) {
    Undo u;
    board.makeMove(move, u);
}

Move Engine::searchBestMove(int maxDepth) {

    stop = false;

    Move bestMove = Move::null();

    for (int depth = 1; depth <= maxDepth; depth++) {
        if (stop && depth > 1) break;
        Move currentBest = findBestMove(board, tt, depth, bestMove, nodes, stop);
        if (!currentBest.isNull()) bestMove = currentBest;
    }

    return bestMove;
}

void Engine::stopSearch() {
    stop = true;
}

const Board& Engine::getBoard() const {
    return board;
}