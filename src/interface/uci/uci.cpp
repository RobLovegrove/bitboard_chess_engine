#include "uci.h"
#include "../../engine/engine.h"
#include "../../game/attacks/attacks.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

void uciLoop() {

    ofstream debugLog("/Users/rob/Documents/CS/Chess/chess_engine/debug.txt", ios::app);

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
        else if (cmd == "ucinewgame") {
            engine.newGame();
        }
        else if (cmd == "position") {
            string type;
            iss >> type;

            if (type == "startpos") {
                engine.newGame();
            } 
            else if (type == "fen") {
                string fen = "";
                string part;

                for (int i = 0; i < 6; i++) {
                    iss >> part;
                    fen += part + " ";
                }

                engine.newGameWithFEN(fen);
            }

            // Handle moves if specified
            string movesWord;
            iss >> movesWord;
            if (movesWord == "moves") {
                string moveStr;
                while (iss >> moveStr) {
                    debugLog << "Opponent Move: " << moveStr << endl;
                    debugLog.flush();
                    engine.makeMove(moveStr);
                }
            }
        }
        else if (cmd == "go") {
            int depth = 5;
            string token;

            while (iss >> token) {
                if (token == "depth") {
                    iss >> depth;
                }
            }

            string best = engine.searchBestMove(depth);
            debugLog << "Engine Move: " << best << endl;
            string boardString = engine.printBoardToString();
            debugLog << boardString << endl;
            debugLog.flush();
            cout << "bestmove " << best << endl;
        }
        else if (cmd == "stop") {
            engine.stopSearch();
        }
        else if (cmd == "quit") {
            break;
        }
    }
}