#include "board.h"
#include "attacks/attacks.h"
#include <iostream>

using namespace std;

bool parseMove(const string& input, int& from, int& to) {
    if (input.length() != 4) return false;
    from = (input[1] - '1') * 8 + (input[0] - 'a');
    to   = (input[3] - '1') * 8 + (input[2] - 'a');
    return true;
}

int main() {

    // Initialise attack look up tables
    initAttacks();

    // Initalise board
    Board board;

    cout << "Perft(1) = " << board.perft(1) << endl;
    cout << "Perft(2) = " << board.perft(2) << endl;
    cout << "Perft(3) = " << board.perft(3) << endl;
    cout << "Perft(4) = " << board.perft(4) << endl;
    // cout << "Perft(5) = " << board.perft(5) << endl;
    // cout << "Perft(6) = " << board.perft(6) << endl;

    string moveStr;
    while (true) {
        board.printBoard();
        cout << board.getSideToMove() << " to move: ";
        cin >> moveStr;
        if (moveStr == "quit" || moveStr == "q") break;

        int from, to;
        if (!parseMove(moveStr, from, to)) {
            cout << "Invalid input. Use e2e4 format." << endl;
            continue;
        }

        Move m{from, to};

        if (!board.isLegalMove(m)) {
            cout << "Move is not a legal chess move" << endl;
        }
        else {
            board.makeMove(m);
        }
    }
    return 0;
}
