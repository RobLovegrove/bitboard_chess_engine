#include "uci.h"
#include "../engine/engine.h"
#include "../game/attacks/attacks.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void uciLoop() {
    Engine engine;
    initAttacks();

    string line;

    while (true) {
        getline(cin, line);
        istringstream iss(line);
        string cmd;
        iss >> cmd;

        if (cmd == "uci") {
            // Engine ID and author
            cout << "id name MyEngine" << endl;
            cout << "id author Rob" << endl;
            cout << "uciok" << endl;
        }
        else if (cmd == "isready") {
            cout << "readyok" << endl;
        }
        else if (cmd == "position") {
            string type;
            iss >> type;

            if (type == "startpos") {
                engine.newGame();
            } 
            else if (type == "fen") {
                string fen, part;
                while (iss >> part) fen += part + " ";
                engine.newGameWithFEN(fen);
            }

            // Handle moves if specified
            string movesWord;
            iss >> movesWord;
            if (movesWord == "moves") {
                string moveStr;
                while (iss >> moveStr) {
                    engine.makeMove(moveStr);
                }
            }
        }
        else if (cmd == "go") {
            int depth = 4; // fixed depth for minimal version
            string best = engine.searchBestMove(depth);
            cout << "bestmove " << best << endl;
        }
        else if (cmd == "quit") {
            break;
        }
    }
}