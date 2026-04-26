#include "../../include/core/CommandProcessor.hpp"
#include "../../include/core/GameController.hpp"
#include "../../include/data/TransactionLogger.hpp"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

CommandProcessor::CommandProcessor(GameController* gc)
    : gameController(gc) {}

void CommandProcessor::processCommand(const string& command) {
    stringstream ss(command);
    string token;
    ss >> token;

    if (token == "START") {
        cout << gameController->startGame() << endl;
    }
    else if (token == "CETAK_LOG") {
        if (!gameController || !gameController->getLogger()) {
            cout << "Logger tidak tersedia.\n";
            return;
        }

        int count = 0;
        vector<string> entries;
        if (ss >> count) {
            if (count < 0) {
                cout << "Format: CETAK_LOG [N], dengan N >= 0.\n";
                return;
            }
            entries = (count == 0)
                ? gameController->getLogger()->getAll()
                : gameController->getLogger()->getRecent(count);
        } else {
            entries = gameController->getLogger()->getAll();
        }

        cout << "[LOG] " << entries.size() << " entri:" << endl;
        for (const string& e : entries) {
            cout << e << endl;
        }
    }
    else if (token == "EXIT") {
        cout << "Keluar dari game.\n";
    }
    else {
        cout << "Command tidak dikenal: " << command << endl;
    }
}