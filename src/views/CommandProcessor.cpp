#include "../../include/views/CommandProcessor.hpp"

#include <iostream>
#include <sstream>

using namespace std;

CommandProcessor::CommandProcessor(GameController* gc, BoardView* bv, PropertyView* pv, GameView* gv)
    : gameController(gc), boardView(bv), propertyView(pv), gameView(gv) {}

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
        if (!gameView || !gameController) return;

        gameView->showDiceRolling();

        dice.rollRandom();

        int d1 = dice.getDie1();
        int d2 = dice.getDie2();
        int total = dice.getTotal();

        gameView->showDiceResult(d1, d2);

        auto player = gameController->getBoard().getCurrentPlayer();
        if (!player) {
            gameView->showError("Tidak ada pemain aktif.");
            return;
        }

        gameView->showPlayerMove(player->getUsername(), total);

        // movement sementara (placeholder)
        int size = gameController->getBoard().getTiles().size();
        int newPos = (player->getPosition() + total) % size;

        player->setPosition(newPos);

        Tile* tile = gameController->getBoard().getTileAt(newPos);
        string name = tile ? tile->getName() : "Unknown";

        gameView->showLanding(name);

        gameView->showMessage("// lanjut ke skenario sesuai jenis tile");
        }
        else if (token == "ATUR_DADU") {
        int d1, d2;
        ss >> d1 >> d2;

        if (d1 < 1 || d1 > 6 || d2 < 1 || d2 > 6) {
            gameView->showInvalidDice();
            return;
    }

    auto player = gameController->getBoard().getCurrentPlayer();

    int total = d1 + d2;
    int newPos = (player->getPosition() + total) % gameController->getBoard().getTiles().size();

    string destination = gameController->getBoard().getTileAt(newPos)->getName();

    gameView->showDiceSetDetailed(player->getUsername(), d1, d2, destination);

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
        cout << "  CETAK_PAPAN     - Menampilkan board game\n";
        cout << "  CETAK_PROPERTI  - Menampilkan detail properti\n";
        cout << "  LEMPAR_DADU     - Melempar dadu secara random\n";
        cout << "  ATUR_DADU [X Y] - Atur hasil dadu secara manual\n";
        cout << "  GADAI           - Menggadaikan properti\n";
        cout << "  TEBUS           - Menebus properti yang digadai\n";
        cout << "  BANGUN          - Membangun rumah/hotel di properti\n";
        cout << "  SIMPAN [file]   - Simpan state game (default savegame.txt)\n";
        cout << "  MUAT   [file]   - Muat state game (default savegame.txt)\n";
        cout << "  CETAK_LOG       - Tampilkan seluruh log transaksi\n";
        cout << "  EXIT            - Keluar dari game\n";
        cout << "  HELP            - Tampilkan help ini\n";
    }
    else {
        cout << "Perintah '" << cmd << "' tidak dikenali. Ketik HELP untuk bantuan." << endl;
    }
}
