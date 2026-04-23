#include "../../include/data/ConfigParser.hpp"

#include "../../include/utils/FileFormatException.hpp"
#include "../../include/utils/CardDeck.hpp"
#include "../../include/utils/ActionCard.hpp"

#include "../../include/models/GameBoard.hpp"
#include "../../include/models/Tile.hpp"
#include "../../include/models/Street.hpp"
#include "../../include/models/Railroad.hpp"
#include "../../include/models/Utility.hpp"
#include "../../include/models/GoTile.hpp"
#include "../../include/models/JailTile.hpp"
#include "../../include/models/FreeParkingTile.hpp"
#include "../../include/models/GoToJailTile.hpp"
#include "../../include/models/ChanceTile.hpp"
#include "../../include/models/CommunityChestTile.hpp"
#include "../../include/models/TaxTile.hpp"
#include "../../include/models/FestivalTile.hpp"

#include <fstream>
#include <cstdlib>
#include <cctype>
#include <memory>

namespace {

int parseInt(const string& s, const string& context) {
    if (s.empty()) {
        throw FileFormatException("Empty integer in " + context);
    }
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (i == 0 && (c == '-' || c == '+')) continue;
        if (c < '0' || c > '9') {
            throw FileFormatException("Non-integer token '" + s + "' in " + context);
        }
    }
    return atoi(s.c_str());
}

string toUpper(const string& s) {
    string out = s;
    for (size_t i = 0; i < out.size(); ++i) {
        out[i] = static_cast<char>(toupper(static_cast<unsigned char>(out[i])));
    }
    return out;
}

bool startsWithDigit(const string& s) {
    return !s.empty() && s[0] >= '0' && s[0] <= '9';
}

}

ConfigParser::ConfigParser(const string& basePath)
    : basePath(basePath),
      pphFlat(0), pphPercentage(0), pbmFlat(0),
      goSalary(0), jailFine(0),
      maxTurn(0), startBalance(0),
      chanceDeck(nullptr), communityDeck(nullptr) {
    chanceDeck = new CardDeck<ActionCard>();
    communityDeck = new CardDeck<ActionCard>();
}

ConfigParser::~ConfigParser() {
    delete chanceDeck;
    delete communityDeck;
}

void ConfigParser::parsePropertyConfig(const string& filename) {
    ifstream in(filename.c_str());
    if (!in.is_open()) {
        throw FileFormatException("Cannot open " + filename);
    }
    string line;
    bool headerConsumed = false;
    while (getline(in, line)) {
        string trimmed = trim(line);
        if (trimmed.empty()) continue;
        vector<string> tokens = tokenizeLine(trimmed);
        if (tokens.empty()) continue;
        if (!headerConsumed) {
            headerConsumed = true;
            if (!startsWithDigit(tokens[0])) continue;
        }
        if (tokens.size() < 5) {
            throw FileFormatException("property.txt: too few fields on line: " + trimmed);
        }

        int id = parseInt(tokens[0], "property.txt ID");
        int position = id - 1;
        string code = tokens[1];
        string name = tokens[2];
        string type = toUpper(tokens[3]);
        string colorRaw = tokens[4];

        Tile* tile = nullptr;
        if (type == "STREET") {
            if (tokens.size() < 10) {
                throw FileFormatException("property.txt: street needs at least 10 fields");
            }
            int harga = parseInt(tokens[5], "property.txt HARGA");
            int gadai = parseInt(tokens[6], "property.txt NILAI_GADAI");
            int upgRumah = parseInt(tokens[7], "property.txt UPG_RUMAH");
            int upgHotel = parseInt(tokens[8], "property.txt UPG_HT");

            string up = toUpper(colorRaw);
            ColorGroup colorGroup;
            if (up == "COKLAT" || up == "COKELAT") colorGroup = COKELAT;
            else if (up == "BIRU_MUDA") colorGroup = BIRU_MUDA;
            else if (up == "MERAH_MUDA" || up == "PINK") colorGroup = PINK;
            else if (up == "ORANGE") colorGroup = ORANGE;
            else if (up == "MERAH") colorGroup = MERAH;
            else if (up == "KUNING") colorGroup = KUNING;
            else if (up == "HIJAU") colorGroup = HIJAU;
            else if (up == "BIRU_TUA") colorGroup = BIRU_TUA;
            else throw FileFormatException("Unknown color group: " + colorRaw);

            Street* s = new Street(position, name, code, colorRaw, colorGroup,
                                   harga, gadai, upgRumah, upgHotel);
            map<int, int> rentTable;
            int levels = static_cast<int>(tokens.size()) - 9;
            if (levels < 1) {
                throw FileFormatException("property.txt: street missing rent levels");
            }
            for (int lvl = 0; lvl < levels; ++lvl) {
                rentTable[lvl] = parseInt(tokens[9 + lvl], "property.txt rent level");
            }
            s->setRentTable(rentTable);
            tile = s;
        } else if (type == "RAILROAD") {
            if (tokens.size() < 7) {
                throw FileFormatException("property.txt: railroad needs 7 fields");
            }
            int harga = parseInt(tokens[5], "property.txt railroad HARGA");
            int gadai = parseInt(tokens[6], "property.txt railroad NILAI_GADAI");
            tile = new Railroad(position, name, code, colorRaw, harga, gadai);
        } else if (type == "UTILITY") {
            if (tokens.size() < 7) {
                throw FileFormatException("property.txt: utility needs 7 fields");
            }
            int harga = parseInt(tokens[5], "property.txt utility HARGA");
            int gadai = parseInt(tokens[6], "property.txt utility NILAI_GADAI");
            tile = new Utility(position, name, code, colorRaw, harga, gadai);
        } else {
            throw FileFormatException("property.txt: unknown property type " + type);
        }
        if (tile != nullptr) tile->setCode(code);
        stagedTiles[position] = tile;
    }
}

void ConfigParser::parseRailroadConfig(const string& filename) {
    ifstream in(filename.c_str());
    if (!in.is_open()) {
        throw FileFormatException("Cannot open " + filename);
    }
    railroadRentTable.clear();
    string line;
    bool headerConsumed = false;
    while (getline(in, line)) {
        string trimmed = trim(line);
        if (trimmed.empty()) continue;
        vector<string> tokens = tokenizeLine(trimmed);
        if (tokens.size() < 2) continue;
        if (!headerConsumed) {
            headerConsumed = true;
            if (!startsWithDigit(tokens[0])) continue;
        }
        int count = parseInt(tokens[0], "railroad.txt JUMLAH");
        int rent = parseInt(tokens[1], "railroad.txt BIAYA_SEWA");
        railroadRentTable[count] = rent;
    }
}

void ConfigParser::parseUtilityConfig(const string& filename) {
    ifstream in(filename.c_str());
    if (!in.is_open()) {
        throw FileFormatException("Cannot open " + filename);
    }
    utilityMultiplierTable.clear();
    string line;
    bool headerConsumed = false;
    while (getline(in, line)) {
        string trimmed = trim(line);
        if (trimmed.empty()) continue;
        vector<string> tokens = tokenizeLine(trimmed);
        if (tokens.size() < 2) continue;
        if (!headerConsumed) {
            headerConsumed = true;
            if (!startsWithDigit(tokens[0])) continue;
        }
        int count = parseInt(tokens[0], "utility.txt JUMLAH");
        int mult = parseInt(tokens[1], "utility.txt FAKTOR");
        utilityMultiplierTable[count] = mult;
    }
}

void ConfigParser::parseTaxConfig(const string& filename) {
    ifstream in(filename.c_str());
    if (!in.is_open()) {
        throw FileFormatException("Cannot open " + filename);
    }
    string line;
    bool headerConsumed = false;
    while (getline(in, line)) {
        string trimmed = trim(line);
        if (trimmed.empty()) continue;
        vector<string> tokens = tokenizeLine(trimmed);
        if (tokens.size() < 3) continue;
        if (!headerConsumed) {
            headerConsumed = true;
            if (!startsWithDigit(tokens[0])) continue;
        }
        pphFlat = parseInt(tokens[0], "tax.txt PPH_FLAT");
        pphPercentage = parseInt(tokens[1], "tax.txt PPH_PERSENTASE");
        pbmFlat = parseInt(tokens[2], "tax.txt PBM_FLAT");
        return;
    }
}

void ConfigParser::parseSpecialConfig(const string& filename) {
    ifstream in(filename.c_str());
    if (!in.is_open()) {
        throw FileFormatException("Cannot open " + filename);
    }
    string line;
    bool headerConsumed = false;
    while (getline(in, line)) {
        string trimmed = trim(line);
        if (trimmed.empty()) continue;
        vector<string> tokens = tokenizeLine(trimmed);
        if (tokens.size() < 2) continue;
        if (!headerConsumed) {
            headerConsumed = true;
            if (!startsWithDigit(tokens[0])) continue;
        }
        goSalary = parseInt(tokens[0], "special.txt GO_SALARY");
        jailFine = parseInt(tokens[1], "special.txt JAIL_FINE");
        return;
    }
}

void ConfigParser::parseMiscConfig(const string& filename) {
    ifstream in(filename.c_str());
    if (!in.is_open()) {
        throw FileFormatException("Cannot open " + filename);
    }
    string line;
    bool headerConsumed = false;
    while (getline(in, line)) {
        string trimmed = trim(line);
        if (trimmed.empty()) continue;
        vector<string> tokens = tokenizeLine(trimmed);
        if (tokens.size() < 2) continue;
        if (!headerConsumed) {
            headerConsumed = true;
            if (!startsWithDigit(tokens[0])) continue;
        }
        maxTurn = parseInt(tokens[0], "misc.txt MAX_TURN");
        startBalance = parseInt(tokens[1], "misc.txt SALDO_AWAL");
        return;
    }
}

void ConfigParser::parseActionConfig(const string& filename) {
    ifstream in(filename.c_str());
    if (!in.is_open()) {
        throw FileFormatException("Cannot open " + filename);
    }
    string line;
    bool headerConsumed = false;
    while (getline(in, line)) {
        string trimmed = trim(line);
        if (trimmed.empty()) continue;
        vector<string> tokens = tokenizeLine(trimmed);
        if (tokens.empty()) continue;
        if (!headerConsumed) {
            headerConsumed = true;
            if (!startsWithDigit(tokens[0])) continue;
        }
        if (tokens.size() < 5) {
            throw FileFormatException("aksi.txt: too few fields on line: " + trimmed);
        }

        int id = parseInt(tokens[0], "aksi.txt ID");
        int position = id - 1;
        string code = tokens[1];
        string name = tokens[2];
        string jenis = toUpper(tokens[3]);
        string color = tokens[4];
        string upCode = toUpper(code);

        Tile* tile = nullptr;
        if (jenis == "SPESIAL" || jenis == "SPECIAL") {
            if (upCode == "GO") {
                tile = new GoTile(position, name, code, color, goSalary);
            } else if (upCode == "PEN" || upCode == "JAIL" || upCode == "PJR") {
                tile = new JailTile(position, name, code, color, jailFine);
            } else if (upCode == "BBP" || upCode == "FP") {
                tile = new FreeParkingTile(position, name, code, color);
            } else if (upCode == "PPJ" || upCode == "GTJ") {
                tile = new GoToJailTile(position, name, code, color);
            } else {
                tile = new FreeParkingTile(position, name, code, color);
            }
        } else if (jenis == "KARTU" || jenis == "CARD") {
            if (upCode == "DNU" || upCode == "DU") {
                tile = new CommunityChestTile(position, name, code, color);
            } else {
                tile = new ChanceTile(position, name, code, color);
            }
        } else if (jenis == "PAJAK" || jenis == "TAX") {
            if (upCode == "PBM") {
                tile = new TaxTile(position, name, code, color, PBM, pbmFlat, 0.0);
            } else {
                tile = new TaxTile(position, name, code, color, PPH, pphFlat,
                                   static_cast<double>(pphPercentage) / 100.0);
            }
        } else if (jenis == "FESTIVAL") {
            tile = new FestivalTile(position, name, code, color);
        } else {
            throw FileFormatException("aksi.txt: unknown JENIS_PETAK " + jenis);
        }
        if (tile != nullptr) tile->setCode(code);
        stagedTiles[position] = tile;
    }
}

int ConfigParser::getMaxTurn() const        { return maxTurn; }
int ConfigParser::getStartBalance() const   { return startBalance; }
int ConfigParser::getGoSalary() const       { return goSalary; }
int ConfigParser::getJailFine() const       { return jailFine; }
int ConfigParser::getPphFlat() const        { return pphFlat; }
int ConfigParser::getPphPercentage() const  { return pphPercentage; }
int ConfigParser::getPbmFlat() const        { return pbmFlat; }

void ConfigParser::loadConfig(GameBoard* board) {
    if (board == nullptr) {
        throw FileFormatException("ConfigParser::loadConfig: board is null");
    }
    const string sep = "/";
    parseTaxConfig(basePath + sep + "tax.txt");
    parseSpecialConfig(basePath + sep + "special.txt");
    parseMiscConfig(basePath + sep + "misc.txt");
    parseRailroadConfig(basePath + sep + "railroad.txt");
    parseUtilityConfig(basePath + sep + "utility.txt");
    parsePropertyConfig(basePath + sep + "property.txt");
    parseActionConfig(basePath + sep + "aksi.txt");

    if (stagedTiles.empty()) {
        throw FileFormatException("No tiles loaded from config");
    }
    int maxPos = stagedTiles.rbegin()->first;
    for (int i = 0; i <= maxPos; ++i) {
        map<int, Tile*>::iterator it = stagedTiles.find(i);
        if (it == stagedTiles.end()) {
            throw FileFormatException("Missing tile at position " + to_string(i));
        }
        board->addTile(std::unique_ptr<Tile>(it->second));
    }
    stagedTiles.clear();

    const vector<std::unique_ptr<Tile>>& tiles = board->getTiles();
    for (size_t i = 0; i < tiles.size(); ++i) {
        Railroad* r = dynamic_cast<Railroad*>(tiles[i].get());
        if (r != nullptr) {
            r->setRentTable(railroadRentTable);
            continue;
        }
        Utility* u = dynamic_cast<Utility*>(tiles[i].get());
        if (u != nullptr) {
            u->setMultiplierTable(utilityMultiplierTable);
        }
    }
}
