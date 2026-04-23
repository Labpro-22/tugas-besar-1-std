#include "../../include/views/CommandProcessor.hpp"
#include "../../include/views/BoardView.hpp"
#include "../../include/views/PropertyView.hpp"
#include "../../include/core/GameController.hpp"
#include "../../include/data/TransactionLogger.hpp"

#include <iostream>
#include <sstream>

using namespace std;

CommandProcessor::CommandProcessor(GameController* gc, BoardView* bv, PropertyView* pv)
    : gameController(gc), boardView(bv), propertyView(pv) {}

void CommandProcessor::readCommand() {
    string input;
    cout << "> ";
    getline(cin, input);

    executeCommand(input);
}

void CommandProcessor::executeCommand(const string& cmd) {
    stringstream ss(cmd);
    string token;
    ss >> token;

    if (token == "CETAK_PAPAN") {
        if (gameController != nullptr && boardView != nullptr) {
            boardView->showBoard(gameController->getBoard(), gameController->getPlayers());
        } else {
            cout << "Error: GameController atau BoardView tidak diinisialisasi." << endl;
        }
    }
    else if (token == "LEMPAR_DADU") {
        cout << "[INFO] Command LEMPAR_DADU belum diimplementasi" << endl;
    }
    else if (token == "ATUR_DADU") {
        cout << "[INFO] Command ATUR_DADU belum diimplementasi" << endl;
    }
    else if (token == "CETAK_AKTA") {
        cout << "[INFO] Command CETAK_AKTUAL belum diimplementasi" << endl;
    }
    else if (token == "CETAK_PROPERTI") {
        cout << "[INFO] Command CETAK_PROPERTI belum diimplementasi" << endl;
    }
    else if (token == "GADAI") {
        cout << "[INFO] Command GADAI belum diimplementasi" << endl;
    }
    else if (token == "TEBUS") {
        cout << "[INFO] Command TEBUS belum diimplementasi" << endl;
    }
    else if (token == "BANGUN") {
        cout << "[INFO] Command BANGUN belum diimplementasi" << endl;
    }
    else if (token == "SIMPAN") {
        string fname;
        ss >> fname;
        if (fname.empty()) fname = "savegame.txt";
        if (gameController != nullptr) gameController->saveGame(fname);
    }
    else if (token == "MUAT") {
        string fname;
        ss >> fname;
        if (fname.empty()) fname = "savegame.txt";
        if (gameController != nullptr) gameController->loadGame(fname);
    }
    else if (token == "CETAK_LOG") {
        if (gameController != nullptr && gameController->getLogger() != nullptr) {
            vector<string> entries = gameController->getLogger()->getAll();
            cout << "[LOG] " << entries.size() << " entri:" << endl;
            for (size_t i = 0; i < entries.size(); ++i) cout << entries[i] << endl;
        }
    }
    else if (token == "GUNAKAN_KEMAMPUAN") {
        cout << "[INFO] Command GUNAKAN_KEMAMPUAN belum diimplementasi" << endl;
    }
    else if (token == "EXIT") {
        cout << "[INFO] Game ditutup." << endl;
        exit(0);
    }
    else if (token == "HELP") {
        cout << "Commands tersedia:\n";
        cout << "  CETAK_PAPAN    - Menampilkan board game\n";
        cout << "  CETAK_PROPERTI - Menampilkan detail properti\n";
        cout << "  SIMPAN [file]  - Simpan state game (default savegame.txt)\n";
        cout << "  MUAT   [file]  - Muat state game (default savegame.txt)\n";
        cout << "  CETAK_LOG      - Tampilkan seluruh log transaksi\n";
        cout << "  EXIT           - Keluar dari game\n";
        cout << "  HELP           - Tampilkan help ini\n";
    }
    else {
        cout << "Perintah '" << cmd << "' tidak dikenali. Ketik HELP untuk bantuan." << endl;
    }
}
