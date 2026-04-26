#include "../include/views/CommandProcessor.hpp"
#include "../include/core/GameController.hpp"
#include "../include/views/BoardView.hpp"
#include "../include/views/PropertyView.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

namespace {

int askIntWithDefault(const string& prompt, int defaultValue) {
    string line;
    cout << prompt;
    getline(cin, line);
    if (line.empty()) return defaultValue;

    try {
        return stoi(line);
    } catch (...) {
        return defaultValue;
    }
}

}

int main() {
    cout << "========================================" << endl;
    cout << "       MONOPOLI GAME - OOP Edition      " << endl;
    cout << "========================================" << endl;
    cout << endl;

    GameController gameController;
    
    BoardView boardView;
    PropertyView propertyView;
    GameView gameView;
    
    CommandProcessor commandProcessor(&gameController, &boardView, &propertyView, &gameView);

    bool initialized = false;
    while (!initialized) {
        cout << "Menu Utama:\n";
        cout << "  1. New Game\n";
        cout << "  2. Load Game\n";
        cout << "  3. Exit\n";
        cout << "Pilih menu [1-3]: ";

        string choice;
        getline(cin, choice);

        if (choice == "1") {
            cout << gameController.startGame() << endl;

            int playerCount = askIntWithDefault("Jumlah pemain [2-4] (default 2): ", 2);
            if (playerCount < 2) playerCount = 2;
            if (playerCount > 4) playerCount = 4;

            int startMoney = askIntWithDefault("Uang awal tiap pemain (default 2000): ", 2000);
            if (startMoney <= 0) startMoney = 2000;

            vector<string> usedNames;
            for (int i = 0; i < playerCount; ++i) {
                string name;
                while (true) {
                    cout << "Nama pemain " << (i + 1) << ": ";
                    getline(cin, name);

                    if (name.empty()) {
                        cout << "Nama tidak boleh kosong.\n";
                        continue;
                    }
                    if (find(usedNames.begin(), usedNames.end(), name) != usedNames.end()) {
                        cout << "Nama sudah dipakai. Gunakan nama lain.\n";
                        continue;
                    }
                    break;
                }

                usedNames.push_back(name);
                cout << gameController.addPlayer(name, startMoney) << endl;
            }

            gameController.initializeGame();
            initialized = true;
        }
        else if (choice == "2") {
            cout << gameController.startGame() << endl;

            cout << "Nama file save (default savegame.txt): ";
            string saveFile;
            getline(cin, saveFile);
            if (saveFile.empty()) saveFile = "savegame.txt";

            if (!gameController.loadGame(saveFile)) {
                cout << "Gagal memuat game dari '" << saveFile << "'.\n\n";
                continue;
            }

            gameController.initializeGame();
            initialized = true;
        }
        else if (choice == "3") {
            cout << "Keluar dari game.\n";
            return 0;
        }
        else {
            cout << "Pilihan tidak valid.\n\n";
        }
    }
    
    cout << "\n[INFO] Ketik HELP untuk melihat command yang tersedia." << endl;
    cout << "[INFO] Mulai dengan CETAK_PAPAN untuk melihat board." << endl;
    cout << endl;
    
    // Main game loop
    while (true) {
        commandProcessor.readCommand();
    }

    return 0;
}
