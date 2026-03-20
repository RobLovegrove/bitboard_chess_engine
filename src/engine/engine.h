#pragma once

#include <string>
#include "../game/board/board.h"

class Engine {
public:
    Engine();

    // Start a new game
    void newGame();

    void newGameWithFEN(const std::string& fenString);

    // Play a move on the board
    void makeMove(const std::string& moveStr);
    void makeMove(Move& moveStr);

    bool parseMove(const std::string& input, int& from, int& to, int& p);

    bool isGameOver(std::string& result); 

    void printBoard() { board.printBoard(); }
    std::string printBoardToString() { return board.printBoardToString(); }

    Colour getSideToMove() { return board.getSideToMove(); }

    // Search for best move
    Move searchBestMove(int depth);
    void stopSearch();

    // Access board (useful for debugging / tests)
    const Board& getBoard() const;

    std::string moveToBK(Move& m) { return board.moveToBK(m); }

    void resetNodes() { nodes = 0; }
    uint64_t getNodes() const { return nodes; }

private:
    Board board;
    bool stop = false;

    uint64_t nodes = 0;

    std::string startPos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    int charToPiece(char c);
};