#include "../src/engine/engine.h"

#include <iostream>

using namespace std;

int main() {

    // Initialise Engine
    Engine engine;

    int depth = 4;

    string sideString;
    cout << "Would you like to play as WHITE (w) or BLACK (b)? ";
    cin >> sideString;
    cout << endl;

    Colour playersColour;
    if (sideString == "quit" || sideString == "q") return 0;
    if (sideString == "b" || sideString == "B") {
        playersColour = BLACK;
        cout << "You are BLACK!" << endl;
    } 
    else if (sideString == "w" || sideString == "W") {
        playersColour = WHITE;
        cout << "You are WHITE!" << endl;
    } 
    else {
        cout << "Invalid input! Defaulting to WHITE." << endl;
        playersColour = WHITE;
    }

    string result = "";
    while (!engine.isGameOver(result)) {

        Colour sideToMove = engine.getSideToMove();

        string move;

        if (sideToMove == playersColour) {

            engine.printBoard();
            cout << "It is your move! ";
            cin >> move;
            if (move == "quit" || move == "q") break;
        }
        else {
            move = engine.searchBestMove(depth);
        }

        try {
            engine.makeMove(move);
            if (sideToMove != playersColour) { 
                cout << "Your opponent has made their move - " << move << endl;
            }
        }
        catch (const runtime_error& e) {
            cout << e.what() << endl;
        } 

    }
    
    if (result != "") {
        engine.printBoard();
        cout << result << "!!" << endl;
    }
    return 0;
}
