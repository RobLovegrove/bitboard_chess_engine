#include "../src/engine/engine.h"
#include "../src/uci/uci.h"

#include <iostream>

using namespace std;

int main() {

    uciLoop();
    return 0;

    // // Initialise Engine
    // Engine engine;
    // engine.newGameWithFEN("4k2r/3q3pp/3BQ2n/1B6/rP6/p7/P1P2PPP/1R2K2R b - - 0 1");

    // int depth = 4;

    // string sideString;
    // cout << "Would you like to play as WHITE (w) or BLACK (b)? ";
    // cin >> sideString;
    // cout << endl;

    // Colour playersColour;
    // if (sideString == "quit" || sideString == "q") return 0;
    // if (sideString == "b" || sideString == "B") {
    //     playersColour = BLACK;
    //     cout << "You are BLACK!" << endl;
    // } 
    // else if (sideString == "w" || sideString == "W") {
    //     playersColour = WHITE;
    //     cout << "You are WHITE!" << endl;
    // } 
    // else {
    //     cout << "Invalid input! Defaulting to WHITE." << endl;
    //     playersColour = WHITE;
    // }

    // string result = "";
    // while (!engine.isGameOver(result)) {

    //     Colour sideToMove = engine.getSideToMove();

    //     string move;

    //     if (sideToMove == playersColour) {

    //         engine.printBoard();
    //         cout << "It is your move! ";
    //         cin >> move;
    //         if (move == "quit" || move == "q") break;
    //     }
    //     else {
    //         move = engine.searchBestMove(depth);
    //     }

    //     try {
    //         engine.makeMove(move);
    //         if (sideToMove != playersColour) { 
    //             cout << "Your opponent has made their move - " << move << endl;
    //         }
    //     }
    //     catch (const runtime_error& e) {
    //         cout << e.what() << endl;
    //         engine.printBoard();
    //         if (sideToMove != playersColour) return 0;
    //     } 

    // }
    
    // if (result != "") {
    //     engine.printBoard();
    //     cout << result << "!!" << endl;
    // }
    // return 0;
}
