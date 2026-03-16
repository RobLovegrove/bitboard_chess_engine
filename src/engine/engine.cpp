#include "engine/engine.h"
#include "search/search.h"
#include "game/attacks/attacks.h"

#include <sstream>

using namespace std;

Engine::Engine() : board(startPos) {
    initAttacks();
}

void Engine::newGame() {
    board = Board(startPos);
}

void Engine::setStartPos(const string& fenString) {
    startPos = fenString;
}

void Engine::newGameWithFEN(const string& fen) {
    board = Board(fen);
}

bool Engine::parseMove(const string& input, int& from, int& to) {
    if (input.length() != 4) return false;
    from = (input[1] - '1') * 8 + (input[0] - 'a');
    to   = (input[3] - '1') * 8 + (input[2] - 'a');
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

    if (!parseMove(moveStr, move.from, move.to)) {
        throw runtime_error("Invalid input: " + moveStr);
    }

    for (auto& m : legalMoves) {
        if (m.from == move.from && m.to == move.to) {
            board.makeMove(m);
            return;
        }
    }

    throw runtime_error("Illegal move: " + moveStr);
}

string Engine::searchBestMove(int depth) {

    Move bestMove = findBestMove(board, depth);

    stringstream ss;
    ss << bestMove;

    return ss.str();
}

const Board& Engine::getBoard() const {
    return board;
}