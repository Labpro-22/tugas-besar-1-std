#include "../../include/views/BoardView.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

pair<string, string> BoardView::formatTile2Line(Tile* tile, const vector<shared_ptr<Player>>& players) {
    if (!tile) return {"        ", "        "};

    string code = tile->getCode();
    string name = tile->getName().substr(0, min((size_t)3, tile->getName().length()));

    string line1 = "[" + code + "] " + name;

    if (line1.length() < 8)
        line1 += string(8 - line1.length(), ' ');
    else
        line1 = line1.substr(0, 8);

    string line2 = "";
    // for (int i = 0; i < (int)players.size(); i++) {
    //     if (players[i]->getPosition() == tile->getPosition()) {
    //         line2 += "(" + to_string(i+1) + ")";
    //     }
    // }

    if (line2.length() < 8)
        line2 += string(8 - line2.length(), ' ');
    else
        line2 = line2.substr(0, 8);

    string color = getColorCode(tile->getColor());

    return {
        color + line1 + "\033[0m",
        color + line2 + "\033[0m"
    };
}

string BoardView::getColorCode(const string& color) {
    if (color == "MERAH") return "\033[31m";
    if (color == "KUNING") return "\033[33m";
    if (color == "HIJAU") return "\033[32m";
    if (color == "BIRU_MUDA") return "\033[36m";
    if (color == "BIRU_TUA") return "\033[34m";
    if (color == "PINK") return "\033[35m";
    if (color == "ORANGE") return "\033[91m";
    if (color == "ABU") return "\033[37m";

    return "\033[37m";
}


string BoardView::getPlayersOnTile(int pos, const vector<shared_ptr<Player>>& players) {
    stringstream ss;

    for (const auto& p : players) {
        if (p != nullptr && p->getPosition() == pos) {
            ss << p->getUsername() << " ";
        }
    }

    return ss.str();
}

void BoardView::printTop(GameBoard& board, const vector<shared_ptr<Player>>& players) {
    cout << "+";
    for (int i = 30; i >= 20; i--) cout << "----------+";
    cout << endl;

    // BARIS 1
    cout << "|";
    for (int i = 30; i >= 20; i--) {
        auto t = formatTile2Line(board.getTileAt(i), players);
        cout << t.first << "|";
    }
    cout << endl;

    // BARIS 2
    cout << "|";
    for (int i = 30; i >= 20; i--) {
        auto t = formatTile2Line(board.getTileAt(i), players);
        cout << t.second << "|";
    }
    cout << endl;
}

void BoardView::printBottom(GameBoard& board, const vector<shared_ptr<Player>>& players) {
    cout << "+";
    for (int i = 0; i <= 10; i++) cout << "----------+";
    cout << endl;

    // BARIS 1
    cout << "|";
    for (int i = 0; i <= 10; i++) {
        auto t = formatTile2Line(board.getTileAt(i), players);
        cout << t.first << "|";
    }
    cout << endl;

    // BARIS 2
    cout << "|";
    for (int i = 0; i <= 10; i++) {
        auto t = formatTile2Line(board.getTileAt(i), players);
        cout << t.second << "|";
    }
    cout << endl;
}


// void BoardView::printCenterInfo(TurnManager& turnManager) {

//     cout << "        ==================================\n";
//     cout << "        ||          NIMONSPOLI          ||\n";
//     cout << "        ==================================\n\n";

//     cout << "                  TURN " + to_string(turnManager.getCurrentTurnNumber()) + " / " + to_string(turnManager.getMaxTurn()) + "\n\n";

//     cout << "        ----------------------------------\n";
//     cout << "        LEGENDA KEPEMILIKAN & STATUS\n";
//     cout << "        P1-P4 : Properti milik Pemain 1-4\n";
//     cout << "        ^  : Rumah level 1\n";
//     cout << "        ^^ : Rumah level 2\n";
//     cout << "        ^^^: Rumah level 3\n";
//     cout << "        *  : Hotel (Maksimal)\n";
//     cout << "        (1)-(4): Bidak (IN=Tahanan, V=Mampir)\n";
//     cout << "        ----------------------------------\n";
//     cout << "        KODE WARNA:\n";
//     cout << "        [CK]=Coklat    [MR]=Merah\n";
//     cout << "        [BM]=Biru Muda [KN]=Kuning\n";
//     cout << "        [PK]=Pink      [HJ]=Hijau\n";
//     cout << "        [OR]=Orange    [BT]=Biru Tua\n";
//     cout << "        [DF]=Aksi      [AB]=Utilitas\n";
// }

void BoardView::printMiddle(GameBoard& board, const vector<shared_ptr<Player>>& players) {

    int left = 39;
    int right = 11;

    for (int i = 0; i < 10; i++) {

        auto L = formatTile2Line(board.getTileAt(left--), players);
        auto R = formatTile2Line(board.getTileAt(right++), players);

        // ===== BARIS 1 =====
        cout << "|";
        cout << L.first;

        // ===== CENTER (langsung cout) =====
        if (i == 0) cout << "        ==================================        ";
        else if (i == 1) cout << "        ||          NIMONSPOLI          ||        ";
        else if (i == 2) cout << "        ==================================        ";
        else if (i == 3) cout << string(72, ' ');
        else if (i == 4) {
            string t = "        TURN " + to_string(board.getCurrentTurnNumber()) +
                       " / " + to_string(board.getMaxTurn());
            cout << t << string(72 - t.length(), ' ');
        }
        else if (i == 5) cout << string(72, ' ');
        else if (i == 6) cout << "        ----------------------------------        ";
        else if (i == 7) cout << "        LEGENDA KEPEMILIKAN & STATUS      ";
        else if (i == 8) cout << "        P1-P4 : Properti milik Pemain     ";
        else if (i == 9) cout << "        (1)-(4): Bidak                    ";

        cout << R.first;
        cout << "|" << endl;

        // ===== BARIS 2 =====
        cout << "|";
        cout << L.second;
        cout << string(90 - L.second.length(), ' ');
        cout << R.second;
        cout << "|" << endl;
    }
}



void BoardView::showBoard(GameBoard& board, const vector<shared_ptr<Player>>& players) {

    printTop(board, players);

    cout << "+--------------------------------------------------------------+\n";

    printMiddle(board, players);

    cout << "+--------------------------------------------------------------+\n";

    printBottom(board, players);
}