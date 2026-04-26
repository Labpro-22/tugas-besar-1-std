#include "../../include/core/WinConditionChecker.hpp"

#include "../../include/models/GameBoard.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/models/Property.hpp"
#include "../../include/models/Tile.hpp"
#include "../../include/utils/SkillCard.hpp"

#include <climits>

WinConditionChecker::WinConditionChecker() : maxTurn(-1) {}

void WinConditionChecker::setMaxTurn(int turns) { maxTurn = turns; }
int  WinConditionChecker::getMaxTurn() const    { return maxTurn; }
bool WinConditionChecker::isMaxTurnValid() const { return maxTurn >= 1; }

int WinConditionChecker::countActivePlayers(GameBoard* board) const {
    if (board == nullptr) return 0;
    int n = 0;
    for (const auto& p : board->getPlayers()) {
        if (p && p->getStatus() != BANKRUPT) ++n;
    }
    return n;
}

int WinConditionChecker::countOwnedProperties(const std::string& username,
                                              GameBoard* board) const {
    if (board == nullptr) return 0;
    int n = 0;
    for (const auto& t : board->getTiles()) {
        Property* p = dynamic_cast<Property*>(t.get());
        if (p != nullptr && p->getOwner() == username) ++n;
    }
    return n;
}

bool WinConditionChecker::isBankruptcyEnd(GameBoard* board) const {
    return countActivePlayers(board) == 1;
}

bool WinConditionChecker::isMaxTurnEnd(GameBoard* board) const {
    if (board == nullptr) return false;
    if (!isMaxTurnValid()) return false;
    return board->getCurrentTurnNumber() > maxTurn;
}

bool WinConditionChecker::isGameOver(GameBoard* board) const {
    return isBankruptcyEnd(board) || isMaxTurnEnd(board);
}

WinMode WinConditionChecker::getEndMode(GameBoard* board) const {
    if (isBankruptcyEnd(board)) return WinMode::BANKRUPTCY;
    if (isMaxTurnEnd(board))    return WinMode::MAX_TURN;
    return WinMode::NONE;
}

std::vector<std::shared_ptr<Player>>
WinConditionChecker::determineWinners(GameBoard* board) const {
    std::vector<std::shared_ptr<Player>> winners;
    if (board == nullptr) return winners;

    std::vector<std::shared_ptr<Player>> eligible;
    for (const auto& p : board->getPlayers()) {
        if (p && p->getStatus() != BANKRUPT) eligible.push_back(p);
    }
    if (eligible.empty()) return winners;

    WinMode mode = getEndMode(board);
    if (mode == WinMode::NONE) return winners;

    if (mode == WinMode::BANKRUPTCY) {
        winners.push_back(eligible.front());
        return winners;
    }

    int maxMoney = INT_MIN;
    for (auto& p : eligible) {
        int m = p->getMoney();
        if (m > maxMoney) maxMoney = m;
    }
    std::vector<std::shared_ptr<Player>> tier1;
    for (auto& p : eligible) {
        if (p->getMoney() == maxMoney) tier1.push_back(p);
    }
    if (tier1.size() == 1) return tier1;

    int maxProps = -1;
    std::vector<int> propCounts;
    propCounts.reserve(tier1.size());
    for (auto& p : tier1) {
        int c = countOwnedProperties(p->getUsername(), board);
        propCounts.push_back(c);
        if (c > maxProps) maxProps = c;
    }
    std::vector<std::shared_ptr<Player>> tier2;
    for (size_t i = 0; i < tier1.size(); ++i) {
        if (propCounts[i] == maxProps) tier2.push_back(tier1[i]);
    }
    if (tier2.size() == 1) return tier2;

    int maxCards = -1;
    std::vector<int> cardCounts;
    cardCounts.reserve(tier2.size());
    for (auto& p : tier2) {
        int c = static_cast<int>(p->getHand().size());
        cardCounts.push_back(c);
        if (c > maxCards) maxCards = c;
    }
    std::vector<std::shared_ptr<Player>> tier3;
    for (size_t i = 0; i < tier2.size(); ++i) {
        if (cardCounts[i] == maxCards) tier3.push_back(tier2[i]);
    }
    return tier3;
}
