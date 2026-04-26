#include "../../include/core/CommandProcessor.hpp"
#include "../../include/core/GameController.hpp"
#include <iostream>

using namespace std;

CommandProcessor::CommandProcessor(GameController* gc)
    : gameController(gc) {}

void CommandProcessor::processCommand(const string& command) {
    if (command == "START") {
        cout << gameController->startGame() << endl;
    }
    else if (command == "CETAK_LOG") {
        cout << "Command CETAK_LOG diterima.\n";
    }
    else if (command == "EXIT") {
        cout << "Keluar dari game.\n";
    }
    else {
        cout << "Command tidak dikenal: " << command << endl;
    }
}