#include "../../include/data/GameSaver.hpp"

#include "../../include/data/TransactionLogger.hpp"
#include "../../include/utils/FileWriteException.hpp"
#include "../../include/utils/SkillCard.hpp"
#include "../../include/utils/CardDeck.hpp"

#include "../../include/models/GameBoard.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/models/Tile.hpp"
#include "../../include/models/Property.hpp"
#include "../../include/models/Street.hpp"
#include "../../include/models/Railroad.hpp"
#include "../../include/models/Utility.hpp"

void GameSaver::writePlayerStates(ofstream& out,
                                  const vector<std::shared_ptr<Player>>& players) const {
    out << players.size() << "\n";
    for (size_t i = 0; i < players.size(); ++i) {
        Player* p = players[i].get();
        if (p == nullptr) continue;

        string statusStr;
        switch (p->getStatus()) {
            case ACTIVE:   statusStr = "ACTIVE";   break;
            case BANKRUPT: statusStr = "BANKRUPT"; break;
            case JAILED:   statusStr = "JAILED";   break;
        }

        const vector<SkillCard*>& hand = p->getHand();
        out << p->getUsername() << " "
            << p->getMoney() << " "
            << p->getPosition() << " "
            << statusStr << " "
            << hand.size();

        for (size_t h = 0; h < hand.size(); ++h) {
            SkillCard* c = hand[h];
            if (c == nullptr) {
                out << " - - -";
                continue;
            }
            string type = c->getCardType();
            out << " " << type;

            bool hasValue = (type == "MoveCard" || type == "DiscountCard" || type == "TeleportCard");
            if (hasValue) out << " " << c->getValue();
            else out << " -";

            bool hasDuration = (type == "DiscountCard");
            if (hasDuration) out << " " << c->getRemainingDuration();
            else out << " -";
        }
        out << "\n";
    }
}

void GameSaver::writeTurnOrder(ofstream& out, GameBoard* board) const {
    if (board == nullptr) {
        out << "\n\n";
        return;
    }
    const vector<std::shared_ptr<Player>>& players = board->getPlayers();
    for (size_t i = 0; i < players.size(); ++i) {
        if (i > 0) out << " ";
        if (players[i] != nullptr) out << players[i]->getUsername();
    }
    out << "\n";
    std::shared_ptr<Player> cur = board->getCurrentPlayer();
    out << (cur ? cur->getUsername() : "") << "\n";
}

void GameSaver::writePropertyStates(ofstream& out,
                                    const vector<std::unique_ptr<Tile>>& tiles) const {
    vector<Property*> props;
    for (size_t i = 0; i < tiles.size(); ++i) {
        Property* p = dynamic_cast<Property*>(tiles[i].get());
        if (p != nullptr) props.push_back(p);
    }
    out << props.size() << "\n";
    for (size_t i = 0; i < props.size(); ++i) {
        Property* p = props[i];

        string kind = "street";
        if (dynamic_cast<Railroad*>(p)) kind = "railroad";
        else if (dynamic_cast<Utility*>(p)) kind = "utility";

        string statusStr;
        switch (p->getStatus()) {
            case BANK:      statusStr = "BANK";      break;
            case OWNED:     statusStr = "OWNED";     break;
            case MORTGAGED: statusStr = "MORTGAGED"; break;
        }

        string owner = p->getOwner();
        if (owner.empty()) owner = "BANK";

        string building = "0";
        Street* s = dynamic_cast<Street*>(p);
        if (s != nullptr) {
            building = s->getBuildingCount();
            if (building.empty()) building = "0";
        }

        string code = p->getCode();
        if (code.empty()) code = to_string(p->getPosition());

        out << code << " " << kind << " " << owner << " "
            << statusStr << " "
            << p->getFestivalMultiplier() << " "
            << p->getFestivalDuration() << " "
            << building << "\n";
    }
}

void GameSaver::writeDeckState(ofstream& out,
                               const vector<SkillCard*>& deck) const {
    out << deck.size();
    for (size_t i = 0; i < deck.size(); ++i) {
        SkillCard* c = deck[i];
        if (c == nullptr) { out << " -"; continue; }
        out << " " << c->getCardType();
    }
    out << "\n";
}

void GameSaver::writeLogState(ofstream& out, TransactionLogger* logger) const {
    if (logger == nullptr) {
        out << 0 << "\n";
        return;
    }
    vector<string> entries = logger->getAll();
    out << entries.size() << "\n";
    for (size_t i = 0; i < entries.size(); ++i) {
        out << entries[i] << "\n";
    }
}

bool GameSaver::save(GameBoard* board, TransactionLogger* logger, const string& filename) const {
    if (board == nullptr) {
        throw FileWriteException("GameSaver::save: board is null");
    }
    ofstream out(filename.c_str());
    if (!out.is_open()) {
        throw FileWriteException("Cannot open " + filename + " for writing");
    }

    out << board->getCurrentTurnNumber() << " " << board->getMaxTurn() << "\n";

    writePlayerStates(out, board->getPlayers());
    writeTurnOrder(out, board);
    writePropertyStates(out, board->getTiles());

    vector<SkillCard*> deck;
    if (board->getSkillDeck() != nullptr) {
        deck = board->getSkillDeck()->getDeck();
    }
    writeDeckState(out, deck);

    writeLogState(out, logger);

    if (!out.good()) {
        throw FileWriteException("Write error on " + filename);
    }
    return true;
}
