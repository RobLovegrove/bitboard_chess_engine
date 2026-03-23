#include <iostream>
#include <iomanip>
#include <string>

#include "../src/game/attacks/attacks.h"
#include "../src/game/board/board.h"
#include "../src/search/search.h"
#include "../src/game/board/zobrist.h"

using namespace std;

struct PerftTest {
    string name;
    Board& position;
    long long targets[6]; // targets for depths 1..6
};

int main() {

    // Initialise attack look up tables
    initAttacks();
    Zobrist::init();

    // -----------------------------
    // Initialize boards
    // -----------------------------
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Board kiwipete("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    Board pos3("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    Board pos4("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    Board pos5("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    Board pos6("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

    // -----------------------------
    // Setup test positions and targets
    // -----------------------------
    PerftTest tests[] = {
        {"Starting position", board, {20, 400, 8902, 197281, 4865609, 119060324}},
        {"Kiwipete", kiwipete, {48, 2039, 97862, 4085603, 193690690, 8031647685}},
        {"Position 3", pos3, {14, 191, 2812, 43238, 674624, 11030083}},
        {"Position 4", pos4, {6, 264, 9467, 422333, 15833292, 706045033}},
        {"Position 5", pos5, {44, 1486, 62379, 2103487, 89941194, -1}},
        {"Position 6", pos6, {46, 2079, 89890, 3894594, 164075551, 6923051137}}
    };

    const int maxDepth = 6;

    cout << "\nPerformance Testing - Depths 1-" << maxDepth << "\n" << endl;

    for (int depth = 1; depth <= maxDepth; ++depth) {
        cout << "Depth " << depth << ":\n";
        cout << left << setw(20) << "Position"
             << setw(12) << "Target"
             << setw(12) << "Result"
             << "Status\n";
        cout << string(50, '-') << endl;

        for (auto &t : tests) {
            long long result = t.position.perft(depth);
            long long target = t.targets[depth - 1];

            if (target == -1) {
                cout << left << setw(20) << t.name
                     << setw(12) << "-"
                    << setw(12) << "-"
                    << "SKIPPED\n";
                continue;
            }

            cout << left << setw(20) << t.name
                 << setw(12) << target
                 << setw(12) << result
                 << ((result == target) ? "✅" : "❌") << endl;
        }

        cout << string(50, '-') << endl;
        cout << "\n";
    }

    return 0;
}