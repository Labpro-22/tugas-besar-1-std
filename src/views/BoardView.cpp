#include "../../include/views/BoardView.hpp"
#include "../../include/models/Property.hpp"
#include "../../include/models/Street.hpp"
#include "../../include/models/Railroad.hpp"
#include "../../include/models/Utility.hpp"
#include "../../include/models/JailTile.hpp"
#include "../../include/models/SpecialTile.hpp"
#include "../../include/models/ActionTile.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

// ANSI color helpers
static const string RESET  = "\033[0m";

string BoardView::getColorCode(const string& color) {
    if (color == "MERAH"      || color == "MR") return "\033[31m";
    if (color == "KUNING"     || color == "KN") return "\033[33m";
    if (color == "HIJAU"      || color == "HJ") return "\033[32m";
    if (color == "BIRU_MUDA"  || color == "BM") return "\033[36m";
    if (color == "BIRU_TUA"   || color == "BT") return "\033[34m";
    if (color == "MERAH_MUDA" || color == "PK" || color == "PINK") return "\033[35m";
    if (color == "ORANGE"     || color == "OR") return "\033[91m";
    if (color == "ABU_ABU"    || color == "AB") return "\033[37m";
    if (color == "COKLAT"     || color == "CK") return "\033[38;5;130m";
    return "\033[37m";
}

// Short color tag e.g. [BT], [MR], [DF]
static string shortColorTag(const string& color) {
    if (color == "MERAH")      return "[MR]";
    if (color == "KUNING")     return "[KN]";
    if (color == "HIJAU")      return "[HJ]";
    if (color == "BIRU_MUDA")  return "[BM]";
    if (color == "BIRU_TUA")   return "[BT]";
    if (color == "MERAH_MUDA" || color == "PINK") return "[PK]";
    if (color == "ORANGE")     return "[OR]";
    if (color == "ABU_ABU")    return "[AB]";
    if (color == "COKLAT")     return "[CK]";
    return "[DF]";
}

// Pad/truncate to exactly `width` visible chars
static string padTo(const string& s, int width) {
    int len = (int)s.size();
    if (len >= width) return s.substr(0, width);
    return s + string(width - len, ' ');
}

// Owner label "P1".."P4" from players list
static string ownerLabel(Property* prop,
                          const vector<shared_ptr<Player>>& players) {
    if (!prop || prop->getStatus() == StatusType::BANK) return "";
    const string& owner = prop->getOwner();
    for (int i = 0; i < (int)players.size(); ++i) {
        if (players[i] && players[i]->getUsername() == owner)
            return "P" + to_string(i + 1);
    }
    return "";
}

// Building indicator ^, ^^, ^^^, ^^^^, *
static string buildingLabel(Street* street) {
    if (!street) return "";
    string bc = street->getBuildingCount();
    if (bc == "H") return "*";
    try {
        int n = stoi(bc);
        if (n > 0) return string(n, '^');
    } catch (...) {}
    return "";
}

// Players at a given board position
string BoardView::getPlayersOnTile(int pos,
                                    const vector<shared_ptr<Player>>& players) {
    string result;
    for (int i = 0; i < (int)players.size(); ++i) {
        if (!players[i] || players[i]->getStatus() == BANKRUPT) continue;
        if (players[i]->getPosition() == pos)
            result += "(" + to_string(i + 1) + ")";
    }
    return result;
}

// Jailed / visiting counts for jail tile
static pair<int,int> jailCounts(int jailPos,
                                 const vector<shared_ptr<Player>>& players) {
    int inJail = 0, visiting = 0;
    for (auto& p : players) {
        if (!p || p->getStatus() == BANKRUPT) continue;
        if (p->getPosition() == jailPos) {
            if (p->getStatus() == JAILED) ++inJail;
            else ++visiting;
        }
    }
    return {inJail, visiting};
}

// Core formatter: returns {line1, line2} each padded to 10 chars.
// line1 already carries ANSI color codes (so actual byte length > 10).
pair<string,string> BoardView::formatTile2Line(
        Tile* tile,
        const vector<shared_ptr<Player>>& players) {

    if (!tile) return {"          ", "          "};

    string color = tile->getColor();
    string tag   = shortColorTag(color);
    string code  = tile->getCode();

    // line1: colored tag + code
    string raw1 = padTo(tag + " " + code, 10);
    string line1 = getColorCode(color) + raw1 + RESET;

    // line2: owner info + player tokens
    string line2;

    // Special case: jail tile shows IN/V counts
    SpecialTile* st = dynamic_cast<SpecialTile*>(tile);
    if (st && st->getSpecialType() == JAIL) {
        auto jc = jailCounts(tile->getPosition(), players);
        int inJ = jc.first;
        int vis = jc.second;
        string ji;
        if (inJ > 0) ji += "IN:" + to_string(inJ) + " ";
        if (vis > 0) ji += "V:"  + to_string(vis);
        line2 = padTo(ji, 10);
        return {line1, line2};
    }

    Property* prop = dynamic_cast<Property*>(tile);
    if (prop && prop->getStatus() != StatusType::BANK) {
        string ol = ownerLabel(prop, players);
        string bl;
        Street* street = dynamic_cast<Street*>(prop);
        if (street) bl = buildingLabel(street);
        string mort = (prop->getStatus() == StatusType::MORTGAGED) ? "[M]" : "";

        if (!ol.empty())   line2 += ol;
        if (!bl.empty())   line2 += " " + bl;
        if (!mort.empty()) line2 += mort;
    }

    // Player tokens (append regardless of property type)
    string pTok = getPlayersOnTile(tile->getPosition(), players);
    if (!pTok.empty()) {
        if (!line2.empty()) line2 += " ";
        line2 += pTok;
    }

    line2 = padTo(line2, 10);
    return {line1, line2};
}

// Legacy single-line formatters (kept so existing callers compile)
string BoardView::formatTile(Tile* tile,
                              const vector<shared_ptr<Player>>& players) {
    return formatTile2Line(tile, players).first;
}

string BoardView::formatTileBottom(Tile* tile,
                                    const vector<shared_ptr<Player>>& players) {
    return formatTile2Line(tile, players).second;
}

// Helpers
static string centerText(const string& s, int width) {
    int len = (int)s.size();
    if (len >= width) return s.substr(0, width);
    int left  = (width - len) / 2;
    int right = width - len - left;
    return string(left, ' ') + s + string(right, ' ');
}

static string hSep(int count) {
    string s = "+";
    for (int i = 0; i < count; ++i) s += "----------+";
    return s;
}

// TOP ROW: indices [side*2 .. side*3], left -> right
void BoardView::printTop(GameBoard& board,
                          const vector<shared_ptr<Player>>& players) {
    int side  = board.getTileCount() / 4;
    int start = side * 2;
    int end   = side * 3;
    int count = end - start + 1;

    cout << hSep(count) << "\n";

    cout << "|";
    for (int i = start; i <= end; ++i) {
        auto pr = formatTile2Line(board.getTileAt(i), players);
        cout << pr.first << "|";
    }
    cout << "\n";

    cout << "|";
    for (int i = start; i <= end; ++i) {
        auto pr = formatTile2Line(board.getTileAt(i), players);
        cout << pr.second << "|";
    }
    cout << "\n";

    cout << hSep(count) << "\n";
}

// BOTTOM ROW: indices [0 .. side], right -> left  (so GO is rightmost)
void BoardView::printBottom(GameBoard& board,
                              const vector<shared_ptr<Player>>& players) {
    int side  = board.getTileCount() / 4;
    int count = side + 1;

    cout << hSep(count) << "\n";

    cout << "|";
    for (int i = side; i >= 0; --i) {
        auto pr = formatTile2Line(board.getTileAt(i), players);
        cout << pr.first << "|";
    }
    cout << "\n";

    cout << "|";
    for (int i = side; i >= 0; --i) {
        auto pr = formatTile2Line(board.getTileAt(i), players);
        cout << pr.second << "|";
    }
    cout << "\n";

    cout << hSep(count) << "\n";
}

// MIDDLE ROWS:
//    Left column  = indices (side*3)+1 .. (side*4)-1  top -> bottom
//    Right column = indices side .. (side*2)-1         bottom -> top
void BoardView::printMiddle(GameBoard& board,
                             const vector<shared_ptr<Player>>& players) {
    int total = board.getTileCount();
    int side  = total / 4;

    vector<int> leftCol, rightCol;
    for (int i = side * 3 + 1; i <= side * 4 - 1; ++i)
        leftCol.push_back(i);
    for (int i = side; i <= side * 2 - 1; ++i)
        rightCol.push_back(i);

    int rows = (int)leftCol.size();
    int cw   = (side - 1) * 11 - 1;

    int maxTurn = board.getMaxTurn();
    string turnStr = "TURN " + to_string(board.getCurrentTurnNumber())
                   + (maxTurn > 0 ? " / " + to_string(maxTurn) : " / -");

    vector<string> centerLines = {
        "==================================",
        "||        NIMONSPOLI            ||",
        "==================================",
        "",
        turnStr,
        "",
        "----------------------------------",
        "LEGENDA KEPEMILIKAN & STATUS",
        "P1-P4 : Properti milik Pemain 1-4",
        "^   : Rumah level 1",
        "^^  : Rumah level 2",
        "^^^ : Rumah level 3",
        "*   : Hotel (Maksimal)",
        "(1)-(4): Bidak Pemain",
        "IN=Tahanan, V=Mampir",
        "----------------------------------",
        "KODE WARNA:",
        "[CK]=Coklat    [MR]=Merah",
        "[BM]=Biru Muda [KN]=Kuning",
        "[PK]=Pink      [HJ]=Hijau",
        "[OR]=Orange    [BT]=Biru Tua",
        "[DF]=Aksi      [AB]=Utilitas"
    };

    int totalLines = rows * 3 - 1;
    int cardH      = (int)centerLines.size();
    int startLine  = (totalLines - cardH) / 2;
    int endLine    = startLine + cardH;

    int gl = 0;

    for (int r = 0; r < rows; ++r) {
        int li = leftCol[r];
        int ri = rightCol[r];

        auto lp = formatTile2Line(board.getTileAt(li), players);
        auto rp = formatTile2Line(board.getTileAt(ri), players);

        {
            string c = (gl >= startLine && gl < endLine)
                     ? centerText(centerLines[gl - startLine], cw)
                     : string(cw, ' ');
            cout << "|" << lp.first << "|" << c << "|" << rp.first << "|\n";
            ++gl;
        }

        {
            string c = (gl >= startLine && gl < endLine)
                     ? centerText(centerLines[gl - startLine], cw)
                     : string(cw, ' ');
            cout << "|" << lp.second << "|" << c << "|" << rp.second << "|\n";
            ++gl;
        }

        if (r < rows - 1) {
            string c = (gl >= startLine && gl < endLine)
                     ? centerText(centerLines[gl - startLine], cw)
                     : string(cw, ' ');
            cout << "+----------+" << c << "+----------+\n";
            ++gl;
        }
    }
}

void BoardView::showBoard(GameBoard& board,
                           const vector<shared_ptr<Player>>& players) {
    if (board.getTileCount() < 4) {
        cout << "[BoardView] Board belum siap (petak < 4).\n";
        return;
    }
    printTop(board, players);
    printMiddle(board, players);
    printBottom(board, players);
}