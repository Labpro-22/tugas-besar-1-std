#include "../../include/views/CommandProcessor.hpp"
#include "../../include/views/BoardView.hpp"
#include "../../include/views/PropertyView.hpp"
#include "../../include/core/GameController.hpp"
#include "../../include/core/TurnManager.hpp"

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
        cout << "[INFO] Command SIMPAN belum diimplementasi" << endl;
    }
    else if (token == "CETAK_LOG") {
        cout << "[INFO] Command CETAK_LOG belum diimplementasi" << endl;
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
        cout << "  EXIT           - Keluar dari game\n";
        cout << "  HELP           - Tampilkan help ini\n";
    }
    else {
        cout << "Perintah '" << cmd << "' tidak dikenali. Ketik HELP untuk bantuan." << endl;
    }
}
