#include "views/CommandProcessor.hpp"
#include "core/GameController.hpp"
#include "views/BoardView.hpp"
#include "views/PropertyView.hpp"
#include "views/GameView.hpp"
#include "data/ConfigParser.hpp"

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

static string readLine(const string& prompt = "") {
    if (!prompt.empty()) cout << prompt;
    string line;
    getline(cin, line);
    while (!line.empty() && (line.back() == '\r' || line.back() == ' '))
        line.pop_back();
    return line;
}

static int readInt(const string& prompt, int defaultVal) {
    string line = readLine(prompt);
    if (line.empty()) return defaultVal;
    try { return stoi(line); } catch (...) { return defaultVal; }
}

static bool setupNewGame(GameController& gc) {
    cout << gc.startGame() << "\n";

    int startMoney = 1500;
    {
        ConfigParser cp("config");
        int sm = cp.getStartBalance();
        if (sm > 0) startMoney = sm;
    }

    int playerCount = readInt("Jumlah pemain [2-4] (default 2): ", 2);
    playerCount = max(2, min(4, playerCount));

    vector<string> usedNames;
    for (int i = 0; i < playerCount; ++i) {
        string name;
        while (true) {
            name = readLine("  Nama pemain " + to_string(i + 1) + ": ");
            if (name.empty()) {
                cout << "  [!] Nama tidak boleh kosong.\n";
                continue;
            }
            if (find(usedNames.begin(), usedNames.end(), name) != usedNames.end()) {
                cout << "  [!] Nama sudah dipakai. Gunakan nama lain.\n";
                continue;
            }
            break;
        }
        usedNames.push_back(name);
        cout << "  " << gc.addPlayer(name, startMoney) << "\n";
    }

    gc.initializeGame();

    cout << "\n[OK] Game dimulai! Setiap pemain mulai dengan M" << startMoney << ".\n";
    return true;
}

static bool setupLoadGame(GameController& gc) {
    cout << gc.startGame() << "\n";

    string saveFile = readLine("Nama file save (default: savegame.txt): ");
    if (saveFile.empty()) saveFile = "savegame.txt";

    if (!gc.loadGame(saveFile)) {
        cout << "[!] Gagal memuat game dari '" << saveFile << "'.\n\n";
        return false;
    }

    gc.initializeGame();
    cout << "[OK] Game berhasil dimuat dari '" << saveFile << "'.\n";
    return true;
}

int main() {

    GameController gameController;
    BoardView      boardView;
    PropertyView   propertyView;
    GameView       gameView;

    CommandProcessor commandProcessor(&gameController, &boardView, &propertyView, &gameView);

    bool initialized = false;
    while (!initialized) {
        cout << "╔══════════════════════════╗\n";
        cout << "║       MENU UTAMA         ║\n";
        cout << "╠══════════════════════════╣\n";
        cout << "║  1. New Game             ║\n";
        cout << "║  2. Load Game            ║\n";
        cout << "║  3. Keluar               ║\n";
        cout << "╚══════════════════════════╝\n";

        string choice = readLine("Pilih [1-3]: ");

        if (choice == "1") {
            initialized = setupNewGame(gameController);
        }
        else if (choice == "2") {
            initialized = setupLoadGame(gameController);
        }
        else if (choice == "3" || choice == "exit" || choice == "EXIT") {
            cout << "Sampai jumpa!\n";
            return 0;
        }
        else {
            cout << "[!] Pilihan tidak valid.\n\n";
        }
    }

    cout << "\n";
    cout << "================================================\n";
    cout << "  Ketik HELP untuk melihat semua perintah.\n";
    cout << "  Mulai dengan CETAK_PAPAN untuk melihat board.\n";
    cout << "================================================\n\n";

    while (true) {
        if (cin.eof()) break;
        commandProcessor.readCommand();
    }

    return 0;
}