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
    if (type == "MOVE" || type == "MoveCard") {
        card = new MoveCard(value);
    } else if (type == "DISCOUNT" || type == "DiscountCard") {
        card = new DiscountCard(value);
    } else if (type == "SHIELD" || type == "ShieldCard") {
        card = new ShieldCard();
    } else if (type == "TELEPORT" || type == "TeleportCard") {
        card = new TeleportCard();
    } else if (type == "LASSO" || type == "LassoCard") {
        card = new LassoCard();
    } else if (type == "DEMOLITION" || type == "DemolitionCard") {
        card = new DemolitionCard();
    } else {
        throw FileFormatException("Unknown skill card type: " + type);
    }
    if (duration >= 0) card->setRemainingDuration(duration);
    return card;
}

}

namespace {
int findPositionByCode(GameBoard* board, const string& code) {
    if (board == nullptr) return -1;
    const vector<std::unique_ptr<Tile>>& tiles = board->getTiles();
    for (size_t k = 0; k < tiles.size(); ++k) {
        if (tiles[k] && tiles[k]->getCode() == code) {
            return static_cast<int>(k);
        }
    }
    return -1;
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
            throw FileFormatException("Save file: missing player header line");
        }
        vector<string> t = tokenizeLine(trim(line));
        if (t.size() < 4) {
            throw FileFormatException("Player header line too short: " + line);
        }
        string username = t[0];
        int money = parseInt(t[1], "player UANG");
        string positionCode = t[2];
        string statusStr = t[3];

        PlayerStatus status;
        int jailTurns = 0;
        if (statusStr == "ACTIVE")        status = ACTIVE;
        else if (statusStr == "BANKRUPT") status = BANKRUPT;
        else if (statusStr == "JAILED")   status = JAILED;
        else if (statusStr.size() > 7 && statusStr.compare(0, 7, "JAILED_") == 0) {
            status = JAILED;
            jailTurns = parseInt(statusStr.substr(7), "player JAILED_N");
        }
        else throw FileFormatException("Unknown player status: " + statusStr);

        int position = findPositionByCode(board, positionCode);
        if (position < 0) {
            bool numeric = !positionCode.empty();
            for (size_t z = 0; z < positionCode.size() && numeric; ++z) {
                char c = positionCode[z];
                if (z == 0 && (c == '-' || c == '+')) continue;
                if (c < '0' || c > '9') numeric = false;
            }
            if (numeric) position = parseInt(positionCode, "player POSISI fallback");
            else throw FileFormatException("Unknown player POSISI_PETAK code: " + positionCode);
        }

        std::shared_ptr<Player> p = std::make_shared<Player>(username, money);
        p->setPosition(position);
        p->setStatus(status);
        for (int k = 0; k < jailTurns; ++k) p->incrementJailTurns();

        if (!readNonBlankLine(in, line)) {
            throw FileFormatException("Save file: missing JUMLAH_KARTU_TANGAN for " + username);
        }
        int handCount = parseInt(trim(line), "JUMLAH_KARTU_TANGAN");

        for (int h = 0; h < handCount; ++h) {
            if (!readNonBlankLine(in, line)) {
                throw FileFormatException("Save file: hand truncated for " + username);
            }
            vector<string> ct = tokenizeLine(trim(line));
            if (ct.empty()) {
                throw FileFormatException("Save file: empty hand-card line for " + username);
            }
            string type = ct[0];
            string valTok = (ct.size() > 1) ? ct[1] : "-";
            string durTok = (ct.size() > 2) ? ct[2] : "-";
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
        throw FileFormatException("Save file: missing deck count line");
    }
    vector<string> hdr = tokenizeLine(trim(line));
    if (hdr.empty()) {
        throw FileFormatException("Save file: empty deck count line");
    }
    int n = parseInt(hdr[0], "JUMLAH_KARTU_DECK_KEMAMPUAN");
    for (int i = 0; i < n; ++i) {
        if (!readNonBlankLine(in, line)) {
            throw FileFormatException("Save file: deck truncated");
        }
        vector<string> t = tokenizeLine(trim(line));
        if (t.empty()) {
            throw FileFormatException("Save file: empty deck card line");
        }
        string type = t[0];
        string valTok = (t.size() > 1) ? t[1] : "-";
        string durTok = (t.size() > 2) ? t[2] : "-";
        SkillCard* c = buildSkillCard(type, valTok, durTok);
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

        string entry = trim(line);
        string::size_type p1 = entry.find(' ');
        if (p1 == string::npos) {
            throw FileFormatException("Log entry malformed: " + line);
        }
        string::size_type p2 = entry.find(' ', p1 + 1);
        if (p2 == string::npos) {
            throw FileFormatException("Log entry malformed (missing username/action): " + line);
        }
        string::size_type p3 = entry.find(' ', p2 + 1);

        int turn = parseInt(entry.substr(0, p1), "log TURN");
        string user = entry.substr(p1 + 1, p2 - p1 - 1);
        string action;
        string detail;
        if (p3 == string::npos) {
            action = entry.substr(p2 + 1);
            detail = "";
        } else {
            action = entry.substr(p2 + 1, p3 - p2 - 1);
            detail = entry.substr(p3 + 1);
        }

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

    board->clearPlayers();
    if (logger != nullptr) logger->clear();

    readPlayerStates(in, board);
    readTurnOrder(in, board);
    readPropertyStates(in, board);

    vector<SkillCard*> loadedDeck = readDeckState(in);
    CardDeck<SkillCard>* targetDeck = board->getSkillDeck();
    if (targetDeck != nullptr) {
        while (!targetDeck->isEmpty()) {
            SkillCard* c = targetDeck->drawTop();
            delete c;
        }
        for (size_t i = 0; i < loadedDeck.size(); ++i) {
            targetDeck->addCard(loadedDeck[i]);
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
