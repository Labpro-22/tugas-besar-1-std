#include "../../include/views/BoardView.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

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

string BoardView::formatTile(Tile* tile, const vector<shared_ptr<Player>>& players) {
    if (!tile) return "          ";

    string code = tile->getCode(); // JKT, BDG, dll
    string color = tile->getColor(); // BIRU_TUA dll

    string shortColor = "[" + color.substr(0,2) + "]"; // [BT], [MR]

    string top = shortColor + " " + code;

    if (top.length() < 10)
        top += string(10 - top.length(), ' ');
    else
        top = top.substr(0, 10);

    return top;
}

string BoardView::formatTileBottom(Tile* tile, const vector<shared_ptr<Player>>& players) {
    if (!tile) return "          ";

    string content = "";

    // contoh: P1
    // nanti bisa ambil owner dari property
    content += "P1 ";

    // contoh: ^^^
    // sementara dummy dulu
    content += "^^ ";

    // pemain di tile
    for (int i = 0; i < players.size(); i++) {
        if (players[i]->getPosition() == tile->getPosition()) {
            content += "(" + to_string(i+1) + ")";
        }
    }

    if (content.length() < 10)
        content += string(10 - content.length(), ' ');
    else
        content = content.substr(0, 10);

    return content;
}
    
string centerText(const string& s, int width) {
    if ((int)s.length() >= width) return s.substr(0, width);

    int left = (width - s.length()) / 2;
    int right = width - s.length() - left;

    return string(left, ' ') + s + string(right, ' ');
}

void BoardView::printTop(GameBoard& board, const vector<shared_ptr<Player>>& players) {

    // int side = board.getTileCount() / 4;
    int side = board.getTileCount() / 4;
    int start = side * 2;
    int end = start + side;

    // border atas
    cout << "+";
    for (int i = start; i <= end; i++) cout << "----------+";
    cout << endl;

    // line atas tile
    cout << "|";
    for (int i = start; i <= end; i++) {
        cout << formatTile(board.getTileAt(i), players) << "|";
    }
    cout << endl;

    // line bawah tile
    cout << "|";
    for (int i = start; i <= end; i++) {
        cout << formatTileBottom(board.getTileAt(i), players) << "|";
    }
    cout << endl;

    // border bawah
    cout << "+";
    for (int i = start; i <= end; i++) cout << "----------+";
    cout << endl;
}

void BoardView::printBottom(GameBoard& board, const vector<shared_ptr<Player>>& players) {

    // int side = board.getTileCount() / 4;
    int side = board.getTileCount() / 4;

    // border atas
    cout << "+";
    for (int i = 0; i <= side; i++) cout << "----------+";
    cout << endl;

    cout << "|";
    for (int i = side; i >= 0; i--) {
        cout << formatTile(board.getTileAt(i), players) << "|";
    }
    cout << endl;

    cout << "|";
    for (int i = side; i >= 0; i--) {
        cout << formatTileBottom(board.getTileAt(i), players) << "|";
    }
    cout << endl;

    // border bawah
    cout << "+";
    for (int i = 0; i <= side; i++) cout << "----------+";
    cout << endl;
}


void BoardView::printMiddle(GameBoard& board, const vector<shared_ptr<Player>>& players) {
    vector<string> centerLines = {
        "==================================",
        "||        NIMONSPOLI            ||",
        "==================================",
        "",
        "TURN " + to_string(board.getCurrentTurnNumber()) + " / " + to_string(board.getMaxTurn()),
        "",
        "----------------------------------",
        "LEGENDA KEPEMILIKAN & STATUS",
        "P1-P4 : Properti milik Pemain 1-4",
        "^  : Rumah level 1",
        "^^ : Rumah level 2",
        "^^^: Rumah level 3",
        "*  : Hotel (Maksimal)",
        "(1)-(4): Bidak (IN=Tahanan, V=Mampir)",
        "----------------------------------",
        "KODE WARNA:",
        "[CK]=Coklat    [MR]=Merah",
        "[BM]=Biru Muda [KN]=Kuning",
        "[PK]=Pink      [HJ]=Hijau",
        "[OR]=Orange    [BT]=Biru Tua",
        "[DF]=Aksi      [AB]=Utilitas"
    };

    // int side = board.getTileCount() / 4;
    int side = board.getTileCount() / 4;
    int middleRows = side - 1;
    int linesPerRow = 3;

    int totalWidth = side * 11 + 1; 
    int centerWidth = totalWidth - (10 + 3);

    int totalLines = middleRows * linesPerRow;
    int cardHeight = centerLines.size();

    int startLine = (totalLines - cardHeight) / 2;
    int endLine = startLine + cardHeight;

    int globalLine = 0;

    for (int i = 0; i < middleRows; i++) {

        int left = 11 + i;
        int right = 31 + i;

        // ===== LINE 1 =====
        string center;
        if (globalLine >= startLine && globalLine < endLine) {
            center = centerText(centerLines[globalLine - startLine], centerWidth);
        } else {
            center = string(centerWidth, ' ');
        }

        cout << "|"
            << formatTile(board.getTileAt(left), players)
            << "|"
            << center
            << "|"
            << formatTile(board.getTileAt(right), players)
            << "|\n";

        globalLine++;

        // ===== LINE 2 =====
        if (globalLine >= startLine && globalLine < endLine) {
            center = centerText(centerLines[globalLine - startLine], centerWidth);
        } else {
            center = string(centerWidth, ' ');
        }

        cout << "|"
            << formatTileBottom(board.getTileAt(left), players)
            << "|"
            << center
            << "|"
            << formatTileBottom(board.getTileAt(right), players)
            << "|\n";

        globalLine++;

        // ===== LINE 3 (separator) =====
        if (i != middleRows - 1) {
            if (globalLine >= startLine && globalLine < endLine) {
                center = centerText(centerLines[globalLine - startLine], centerWidth);
            } else {
                center = string(centerWidth, ' ');
            }

            cout << "+----------+"
                << center
                << "+----------+\n";

            globalLine++;
        }
    }
}



void BoardView::showBoard(GameBoard& board, const vector<shared_ptr<Player>>& players) {
    printTop(board, players);
    printMiddle(board, players);
    printBottom(board, players);
}