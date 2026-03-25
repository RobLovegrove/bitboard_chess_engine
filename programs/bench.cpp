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

string currentTimestamp() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    return string(buf);
}

string gitCommitHash() {
    string hash;
    FILE* pipe = popen("git rev-parse --short HEAD", "r");
    if (!pipe) return "unknown";
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), pipe)) {
        hash = buffer;
        hash.pop_back();
    }
    pclose(pipe);
    return hash;
}

int main() {
    Engine engine;
    vector<int> depths = {2, 4, 6};

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
    csv << "CompileFlags,-O3 -std=c++17\n\n";
    csv << "Depth,FEN,ExpectedMoves,BestMove,Correct,Time_s,Nodes,NPS\n";

    // Summary tracking
    int totalPositions = 0;
    map<int, int>      correctByDepth;
    map<int, int>      totalByDepth;
    map<int, double>   timeByDepth;
    map<int, uint64_t> nodesByDepth;

    for (int d : depths) {
        correctByDepth[d] = 0;
        totalByDepth[d]   = 0;
        timeByDepth[d]    = 0.0;
        nodesByDepth[d]   = 0;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        vector<string> tokens;
        string token;
        while (ss >> token) tokens.push_back(token);
        if (tokens.size() < 6) continue;

        // Reconstruct FEN
        string fenPart = "";
        for (int i = 0; i < 6; ++i) fenPart += tokens[i] + " ";
        fenPart.pop_back();

        vector<string> expectedMoves;
        for (size_t i = 6; i < tokens.size(); ++i) {
            if (tokens[i] == "bm") continue;
            expectedMoves.push_back(tokens[i]);
        }

        engine.newGameWithFEN(fenPart);
        totalPositions++;

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

            // Accumulate summary stats
            correctByDepth[maxDepth] += correct ? 1 : 0;
            totalByDepth[maxDepth]++;
            timeByDepth[maxDepth]  += elapsed;
            nodesByDepth[maxDepth] += nodes;

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
            cout << "| Best: "    << moveString
                 << " | Correct: " << (correct ? "✅" : "❌")
                 << " | Time: "    << elapsed << "s"
                 << " | NPS: "     << nps
                 << endl;
        }
    }

    // Console summary
    cout << "\n========== BENCHMARK SUMMARY ==========\n";
    cout << "Timestamp:  " << currentTimestamp() << "\n";
    cout << "Commit:     " << gitCommitHash() << "\n";
    cout << "Positions:  " << totalPositions << "\n\n";

    cout << left
         << setw(8)  << "Depth"
         << setw(12) << "Correct"
         << setw(12) << "Time(s)"
         << setw(14) << "Nodes"
         << setw(12) << "NPS"
         << "\n";
    cout << string(58, '-') << "\n";

    for (int d : depths) {
        double nps = nodesByDepth[d] / timeByDepth[d];
        cout << left
             << setw(8)  << d
             << correctByDepth[d] << "/" << setw(8) << totalByDepth[d]
             << setw(12) << fixed << setprecision(3) << timeByDepth[d]
             << setw(14) << nodesByDepth[d]
             << setw(12) << fixed << setprecision(0) << nps
             << "\n";
    }
    cout << "========================================\n";

    // CSV summary
    csv << "\nSUMMARY\n";
    csv << "Depth,Correct,Total,TotalTime_s,TotalNodes,AvgNPS\n";
    for (int d : depths) {
        double nps = nodesByDepth[d] / timeByDepth[d];
        csv << d << ","
            << correctByDepth[d] << ","
            << totalByDepth[d]   << ","
            << fixed << setprecision(6) << timeByDepth[d] << ","
            << nodesByDepth[d]   << ","
            << fixed << setprecision(2) << nps << "\n";
    }

    cout << "\nBenchmark results saved to " << csvFile << endl;
    return 0;
}