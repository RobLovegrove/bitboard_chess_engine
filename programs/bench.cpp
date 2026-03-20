#include "../src/engine/engine.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <chrono>
#include <map>
#include <iomanip>
#include <ctime>
#include <cstdio>

using namespace std;

// Helper to get current timestamp
string currentTimestamp() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    return string(buf);
}

// Helper to get current git commit hash
string gitCommitHash() {
    string hash;
    FILE* pipe = popen("git rev-parse --short HEAD", "r");
    if (!pipe) return "unknown";
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), pipe)) {
        hash = buffer;
        hash.pop_back(); // remove newline
    }
    pclose(pipe);
    return hash;
}

int main() {

    Engine engine;
    vector<int> depths = {2, 4, 6};   // Depths
    ifstream file("programs/positions.txt");
    if (!file.is_open()) {
        cerr << "Failed to open positions.txt" << endl;
        return 1;
    }

    string csvFile = "bench_results.csv";
    ofstream csv(csvFile);
    if (!csv.is_open()) {
        cerr << "Failed to open " << csvFile << " for writing" << endl;
        return 1;
    }

    // CSV header metadata
    csv << "Timestamp," << currentTimestamp() << "\n";
    csv << "EngineVersion," << gitCommitHash() << "\n";
    csv << "CompileFlags," << "-O3 -std=c++17\n\n";
    csv << "Depth,FEN,ExpectedMoves,BestMove,Correct,Time_s,Nodes,NPS\n";

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        string fenPart;
        vector<string> tokens;
        string token;

        while (ss >> token) tokens.push_back(token);
        if (tokens.size() < 6) continue; // invalid FEN

        // Reconstruct FEN
        fenPart = "";
        for (int i = 0; i < 6; ++i) fenPart += tokens[i] + " ";
        fenPart.pop_back();

        vector<string> expectedMoves;
        for (size_t i = 6; i < tokens.size(); ++i) {
            if (tokens[i] == "bm") continue; // skip literal
            expectedMoves.push_back(tokens[i]);
        }

        engine.newGameWithFEN(fenPart);

        for (int maxDepth : depths) {

            engine.resetNodes();
            auto start = chrono::high_resolution_clock::now();
            Move bestMove = engine.searchBestMove(maxDepth);
            auto end = chrono::high_resolution_clock::now();
            double elapsed = chrono::duration<double>(end - start).count();

            string moveString = engine.moveToBK(bestMove);

            bool correct = false;
            for (auto& em : expectedMoves) {
                if (moveString == em) { correct = true; break; }
            }

            uint64_t nodes = engine.getNodes();
            double nps = nodes / elapsed;

            // Write CSV row
            csv << maxDepth << ",\"" << fenPart << "\",\"";
            for (auto& m : expectedMoves) csv << m << " ";
            csv << "\"," << moveString << "," << (correct ? "1" : "0")
                << "," << fixed << setprecision(6) << elapsed
                << "," << nodes << "," << fixed << setprecision(2) << nps << "\n";

            // Print to console
            cout << "Depth: " << maxDepth 
                 << " | Expected: ";
            for (auto& m : expectedMoves) cout << m << " ";
            cout << "| Best: " << moveString
                 << " | Correct: " << (correct ? "✅" : "❌")
                 << " | Time: " << elapsed << "s"
                 << " | NPS: " << nps
                 << endl;
        }
    }

    cout << "\nBenchmark results saved to " << csvFile << endl;

    return 0;
}