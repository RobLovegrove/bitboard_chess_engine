#include "board/board.h"
#include "attacks/attacks.h"
#include "search/search.h"

#include <iostream>

using namespace std;

const string START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

bool parseMove(const string& input, int& from, int& to) {
    if (input.length() != 4) return false;
    from = (input[1] - '1') * 8 + (input[0] - 'a');
    to   = (input[3] - '1') * 8 + (input[2] - 'a');
    return true;
}

int main() {

    // Initialise attack look up tables
    initAttacks();

    // // Initalise board
    Board board(START_POS);
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


    string moveStr;
    while (true) {

        board.printBoard();
        vector<Move> moves = board.generateLegalMoves();
        if (moves.empty()) {
            if (board.isKingInCheck(board.getSideToMove())) {
                cout << "CHECKMATE" << endl;
            }
            else {
                cout << "STALEMATE" << endl;
            }
            break;
        }
        
        Colour sideToMove = board.getSideToMove();

        if (sideToMove == playersColour) {
            cout << "It is your move! ";
            cin >> moveStr;
            if (moveStr == "quit" || moveStr == "q") break;

            int from, to;
            if (!parseMove(moveStr, from, to)) {
                cout << "Invalid input. Use e2e4 format." << endl;
                continue;
            }

            Move m{from, to};

            if (!board.isLegalMove(m, moves)) {
                cout << "Move is not a legal chess move" << endl;
            }
            else {
                board.makeMove(m);
            }
        }
        else {
            Move bestMove = findBestMove(board, depth);
            board.makeMove(bestMove);
            cout << "Your opponent has made their move - " << bestMove << endl;
        }

    }

    // cout << "Perft(1) = " << board.perft(1) << endl;
    // cout << "Perft(2) = " << board.perft(2) << endl;
    // cout << "Perft(3) = " << board.perft(3) << endl;
    // cout << "Perft(4) = " << board.perft(4) << endl;
    // cout << "Perft(5) = " << board.perft(5) << endl;
    // //cout << "Perft(6) = " << board.perft(6) << endl;
    // //cout << "Perft(7) = " << board.perft(7) << endl;

    // Board kiwipete("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    
    // cout << "kiwipete perf testing" << endl;
    // cout << "Perft(1) = " << kiwipete.perft(1) << endl;
    // cout << "Perft(2) = " << kiwipete.perft(2) << endl;
    // cout << "Perft(3) = " << kiwipete.perft(3) << endl;
    // cout << "Perft(4) = " << kiwipete.perft(4) << endl;
    // cout << "Perft(5) = " << kiwipete.perft(5) << endl;
    // cout << "Perft(6) = " << kiwipete.perft(6) << endl;

    // kiwipete.printBoard();

    // const uint64_t* bitboards = kiwipete.getAllBitboards();

    // for (int i = 0; i < 12; i++) {
    //     cout << "Bitboard for " << static_cast<PieceType>(i) << endl;
    //     kiwipete.printBitboard(bitboards[i]);
    // }

    // Board pos3("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    
    // cout << "pos3 perf testing" << endl;
    // cout << "Perft(1) = " << pos3.perft(1) << endl;
    // cout << "Perft(2) = " << pos3.perft(2) << endl;
    // cout << "Perft(3) = " << pos3.perft(3) << endl;
    // cout << "Perft(4) = " << pos3.perft(4) << endl;
    // cout << "Perft(5) = " << pos3.perft(5) << endl;
    // cout << "Perft(6) = " << pos3.perft(6) << endl;

    // Board pos4("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    
    // cout << "pos4 perf testing" << endl;
    // cout << "Perft(1) = " << pos4.perft(1) << endl;
    // cout << "Perft(2) = " << pos4.perft(2) << endl;
    // cout << "Perft(3) = " << pos4.perft(3) << endl;
    // cout << "Perft(4) = " << pos4.perft(4) << endl;
    // cout << "Perft(5) = " << pos4.perft(5) << endl;
    // cout << "Perft(6) = " << pos4.perft(6) << endl;

    // Board pos5("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    
    // cout << "pos5 perf testing" << endl;
    // cout << "Perft(1) = " << pos5.perft(1) << endl;
    // cout << "Perft(2) = " << pos5.perft(2) << endl;
    // cout << "Perft(3) = " << pos5.perft(3) << endl;
    // cout << "Perft(4) = " << pos5.perft(4) << endl;
    // cout << "Perft(5) = " << pos5.perft(5) << endl;
    // cout << "Perft(6) = " << pos5.perft(6) << endl;

    // Board pos6("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    
    // cout << "pos6 perf testing" << endl;
    // cout << "Perft(1) = " << pos6.perft(1) << endl;
    // cout << "Perft(2) = " << pos6.perft(2) << endl;
    // cout << "Perft(3) = " << pos6.perft(3) << endl;
    // cout << "Perft(4) = " << pos6.perft(4) << endl;
    // cout << "Perft(5) = " << pos6.perft(5) << endl;
    // cout << "Perft(6) = " << pos6.perft(6) << endl;

    return 0;
}
