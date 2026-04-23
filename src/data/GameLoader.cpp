#include "../../include/data/GameLoader.hpp"

#include "../../include/data/TransactionLogger.hpp"
#include "../../include/utils/FileFormatException.hpp"
#include "../../include/utils/SkillCard.hpp"
#include "../../include/utils/CardDeck.hpp"

#include "../../include/models/GameBoard.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/models/Tile.hpp"
#include "../../include/models/Property.hpp"
#include "../../include/models/Street.hpp"

#include <cstdlib>
#include <cctype>

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

bool readNonBlankLine(ifstream& in, string& out) {
    string line;
    while (getline(in, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }
        bool allWhite = true;
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] != ' ' && line[i] != '\t') { allWhite = false; break; }
        }
        if (line.empty() || allWhite) continue;
        out = line;
        return true;
    }
    return false;
}

SkillCard* buildSkillCard(const string& type, const string& valTok, const string& durTok) {
    int value = (valTok == "-" || valTok.empty()) ? 0
                : parseInt(valTok, "skill card value");
    int duration = (durTok == "-" || durTok.empty()) ? -1
                   : parseInt(durTok, "skill card duration");

    SkillCard* card = nullptr;
    if (type == "MoveCard") {
        card = new MoveCard(value);
    } else if (type == "DiscountCard") {
        card = new DiscountCard(value);
    } else if (type == "ShieldCard") {
        card = new ShieldCard();
    } else if (type == "TeleportCard") {
        card = new TeleportCard(value);
    } else if (type == "LassoCard") {
        card = new LassoCard();
    } else if (type == "DemolitionCard") {
        card = new DemolitionCard();
    } else {
        throw FileFormatException("Unknown skill card type: " + type);
    }
    if (duration >= 0) card->setRemainingDuration(duration);
    return card;
}

}

void GameLoader::readPlayerStates(ifstream& in, GameBoard* board) {
    string line;
    if (!readNonBlankLine(in, line)) {
        throw FileFormatException("Save file: missing player count");
    }
    vector<string> hdr = tokenizeLine(trim(line));
    if (hdr.empty()) {
        throw FileFormatException("Save file: empty player count line");
    }
    int n = parseInt(hdr[0], "JUMLAH_PEMAIN");
    for (int i = 0; i < n; ++i) {
        if (!readNonBlankLine(in, line)) {
            throw FileFormatException("Save file: missing player line");
        }
        vector<string> t = tokenizeLine(trim(line));
        if (t.size() < 5) {
            throw FileFormatException("Player line too short: " + line);
        }
        string username = t[0];
        int money = parseInt(t[1], "player UANG");
        int position = parseInt(t[2], "player POSISI");
        string statusStr = t[3];
        int handCount = parseInt(t[4], "player JUMLAH_KARTU");

        PlayerStatus status;
        if (statusStr == "ACTIVE")        status = ACTIVE;
        else if (statusStr == "BANKRUPT") status = BANKRUPT;
        else if (statusStr == "JAILED")   status = JAILED;
        else throw FileFormatException("Unknown player status: " + statusStr);

        std::shared_ptr<Player> p = std::make_shared<Player>(username, money);
        p->setPosition(position);
        p->setStatus(status);

        size_t idx = 5;
        for (int h = 0; h < handCount; ++h) {
            if (idx + 2 >= t.size()) {
                throw FileFormatException("Player hand truncated for " + username);
            }
            string type = t[idx];
            string valTok = t[idx + 1];
            string durTok = t[idx + 2];
            idx += 3;
            SkillCard* c = buildSkillCard(type, valTok, durTok);
            p->receiveCard(c);
        }

        board->addPlayer(p);
    }
}

void GameLoader::readTurnOrder(ifstream& in, GameBoard* board) {
    string line;
    if (!readNonBlankLine(in, line)) {
        throw FileFormatException("Save file: missing turn order");
    }
    vector<string> order = tokenizeLine(trim(line));
    if (board != nullptr && !order.empty()) {
        board->setTurnOrder(order);
    }

    if (!readNonBlankLine(in, line)) {
        throw FileFormatException("Save file: missing current player");
    }
    string currentUser = trim(line);
    if (board != nullptr && !currentUser.empty()) {
        board->setCurrentPlayerByUsername(currentUser);
    }
}

void GameLoader::readPropertyStates(ifstream& in, GameBoard* board) {
    string line;
    if (!readNonBlankLine(in, line)) {
        throw FileFormatException("Save file: missing property count");
    }
    vector<string> hdr = tokenizeLine(trim(line));
    if (hdr.empty()) {
        throw FileFormatException("Save file: empty property count");
    }
    int n = parseInt(hdr[0], "JUMLAH_PROPERTI");
    for (int i = 0; i < n; ++i) {
        if (!readNonBlankLine(in, line)) {
            throw FileFormatException("Save file: missing property line");
        }
        vector<string> t = tokenizeLine(trim(line));
        if (t.size() < 7) {
            throw FileFormatException("Property line too short: " + line);
        }
        string code = t[0];
        string owner = t[2];
        string statusStr = t[3];
        int fmult = parseInt(t[4], "FMULT");
        int fdur = parseInt(t[5], "FDUR");
        string building = t[6];

        Property* prop = nullptr;
        const vector<std::unique_ptr<Tile>>& tiles = board->getTiles();
        for (size_t k = 0; k < tiles.size(); ++k) {
            Property* cand = dynamic_cast<Property*>(tiles[k].get());
            if (cand != nullptr && cand->getCode() == code) {
                prop = cand;
                break;
            }
        }
        if (prop == nullptr) {
            throw FileFormatException("Save file references unknown property code: " + code);
        }

        StatusType status;
        if (statusStr == "BANK")           status = BANK;
        else if (statusStr == "OWNED")     status = OWNED;
        else if (statusStr == "MORTGAGED") status = MORTGAGED;
        else throw FileFormatException("Unknown property status: " + statusStr);

        prop->setOwner(owner == "BANK" ? "" : owner);
        prop->setStatus(status);
        prop->setFestivalMultiplier(fmult);
        prop->setFestivalDuration(fdur);

        Street* s = dynamic_cast<Street*>(prop);
        if (s != nullptr) {
            s->setBuildingCount(building);
        }
    }
}

vector<SkillCard*> GameLoader::readDeckState(ifstream& in) {
    vector<SkillCard*> out;
    string line;
    if (!readNonBlankLine(in, line)) {
        throw FileFormatException("Save file: missing deck line");
    }
    vector<string> t = tokenizeLine(trim(line));
    if (t.empty()) {
        throw FileFormatException("Save file: empty deck line");
    }
    int n = parseInt(t[0], "JUMLAH_KARTU_DECK");
    for (int i = 0; i < n; ++i) {
        if (static_cast<int>(t.size()) < 1 + i + 1) {
            throw FileFormatException("Save file: deck truncated");
        }
        SkillCard* c = buildSkillCard(t[1 + i], "-", "-");
        out.push_back(c);
    }
    return out;
}

void GameLoader::readLogState(ifstream& in, TransactionLogger* logger) {
    string line;
    if (!readNonBlankLine(in, line)) {
        throw FileFormatException("Save file: missing log count");
    }
    vector<string> hdr = tokenizeLine(trim(line));
    if (hdr.empty()) {
        throw FileFormatException("Save file: empty log count");
    }
    int n = parseInt(hdr[0], "JUMLAH_ENTRI_LOG");

    for (int i = 0; i < n; ++i) {
        if (!readNonBlankLine(in, line)) {
            throw FileFormatException("Save file: missing log entry");
        }
        if (logger == nullptr) continue;

        string::size_type lb = line.find('[');
        string::size_type rb = line.find(']');
        if (lb == string::npos || rb == string::npos || rb <= lb) {
            throw FileFormatException("Log entry missing [Turn N] prefix: " + line);
        }
        string inside = line.substr(lb + 1, rb - lb - 1);
        string::size_type sp = inside.find(' ');
        if (sp == string::npos) {
            throw FileFormatException("Log entry malformed turn: " + line);
        }
        int turn = parseInt(inside.substr(sp + 1), "log TURN");

        string rest = line.substr(rb + 1);
        size_t k = 0;
        while (k < rest.size() && rest[k] == ' ') ++k;
        rest = rest.substr(k);

        string::size_type p1 = rest.find(" | ");
        if (p1 == string::npos) {
            throw FileFormatException("Log entry missing separator: " + line);
        }
        string user = rest.substr(0, p1);
        string after1 = rest.substr(p1 + 3);
        string::size_type p2 = after1.find(" | ");
        if (p2 == string::npos) {
            throw FileFormatException("Log entry missing second separator: " + line);
        }
        string action = after1.substr(0, p2);
        string detail = after1.substr(p2 + 3);

        logger->log(turn, user, action, detail);
    }
}

bool GameLoader::loadSave(const string& filename,
                          GameBoard* board,
                          TransactionLogger* logger) {
    if (board == nullptr) {
        throw FileFormatException("GameLoader::loadSave: board is null");
    }
    ifstream in(filename.c_str());
    if (!in.is_open()) {
        throw FileFormatException("Cannot open " + filename);
    }

    string line;
    if (!readNonBlankLine(in, line)) {
        throw FileFormatException("Save file empty");
    }
    vector<string> hdr = tokenizeLine(trim(line));
    if (hdr.size() < 2) {
        throw FileFormatException("Save file: first line needs <TURN> <MAX_TURN>");
    }
    int currentTurn = parseInt(hdr[0], "TURN");
    int maxTurn = parseInt(hdr[1], "MAX_TURN");
    board->setCurrentTurnNumber(currentTurn);
    board->setMaxTurn(maxTurn);

    readPlayerStates(in, board);
    readTurnOrder(in, board);
    readPropertyStates(in, board);

    vector<SkillCard*> loadedDeck = readDeckState(in);
    CardDeck<SkillCard*>* targetDeck = board->getSkillDeck();
    if (targetDeck != nullptr) {
        while (!targetDeck->isEmpty()) {
            SkillCard* c = targetDeck->drawTop();
            delete c;
        }
        for (size_t i = 0; i < loadedDeck.size(); ++i) {
            targetDeck->addToDeck(loadedDeck[i]);
        }
    } else {
        for (size_t i = 0; i < loadedDeck.size(); ++i) {
            delete loadedDeck[i];
        }
    }

    readLogState(in, logger);

    return true;
}

bool GameLoader::validate(const string& filename) {
    ifstream in(filename.c_str());
    if (!in.is_open()) return false;

    string line;
    if (!readNonBlankLine(in, line)) return false;
    vector<string> header = tokenizeLine(trim(line));
    if (header.size() < 2) return false;
    for (size_t i = 0; i < 2; ++i) {
        const string& t = header[i];
        if (t.empty()) return false;
        for (size_t j = 0; j < t.size(); ++j) {
            if (!isdigit(static_cast<unsigned char>(t[j]))) return false;
        }
    }
    if (!readNonBlankLine(in, line)) return false;
    vector<string> second = tokenizeLine(trim(line));
    if (second.empty()) return false;
    for (size_t j = 0; j < second[0].size(); ++j) {
        if (!isdigit(static_cast<unsigned char>(second[0][j]))) return false;
    }
    return true;
}
