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

    bool parseMove(const std::string& input, int& from, int& to, int& p);

    bool isGameOver(std::string& result); 

    void printBoard() { board.printBoard(); }
    std::string printBoardToString() { return board.printBoardToString(); }

    Colour getSideToMove() { return board.getSideToMove(); }

    // Search for best move
    std::string searchBestMove(int depth);
    void stopSearch();

    // Access board (useful for debugging / tests)
    const Board& getBoard() const;

private:
    Board board;
    bool stop = false;

    std::string startPos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    int charToPiece(char c);
};