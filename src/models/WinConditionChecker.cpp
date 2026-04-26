#include "../include/models/WinConditionChecker.hpp"
#include "../include/models/Player.hpp"
#include "../include/core/TurnManager.hpp"
#include "../include/models/GameBoard.hpp"
#include "../include/models/Property.hpp"
#include "../include/models/Tile.hpp"

#include <algorithm>

WinConditionChecker::WinConditionChecker(TurnManager* tm, GameBoard* board)
    : turnManager(tm), gameBoard(board) {}

int WinConditionChecker::countActive(const std::vector<std::shared_ptr<Player>>& players) const {
    int count = 0;
    for (const auto& p : players) {
        if (p && p->getStatus() != BANKRUPT) count++;
    }
    return count;
}

bool WinConditionChecker::isGameOver(const std::vector<std::shared_ptr<Player>>& players) const {
    // Kondisi 1: hanya 1 pemain aktif
    if (countActive(players) <= 1) return true;

    // Kondisi 2: batas giliran tercapai
    if (turnManager && turnManager->isMaxTurnReached()) return true;

    return false;
}

Player* WinConditionChecker::determineWinner(const std::vector<std::shared_ptr<Player>>& players) const {
    std::vector<Player*> winners = determineWinners(players);
    if (winners.empty()) return nullptr;
    return winners.front();
}

std::vector<Player*> WinConditionChecker::determineWinners(const std::vector<std::shared_ptr<Player>>& players) const {
    // Jika hanya 1 aktif, dia langsung menang
    Player* lastActive = nullptr;
    int activeCnt = 0;
    for (const auto& p : players) {
        if (p && p->getStatus() != BANKRUPT) {
            lastActive = p.get();
            activeCnt++;
        }
    }
    if (activeCnt == 1) return { lastActive };
    if (activeCnt == 0) return {};

    std::vector<Player*> activePlayers;
    int maxWealth = -1;
    for (const auto& p : players) {
        if (!p || p->getStatus() == BANKRUPT) continue;
        int wealth = p->calculateTotalWealth();
        maxWealth = std::max(maxWealth, wealth);
    }
    for (const auto& p : players) {
        if (!p || p->getStatus() == BANKRUPT) continue;
        if (p->calculateTotalWealth() == maxWealth) activePlayers.push_back(p.get());
    }
    if (activePlayers.size() <= 1) return activePlayers;

    // Tie-break #1: jumlah properti
    std::vector<Player*> propertyLeaders;
    int maxPropertyCount = -1;
    for (Player* candidate : activePlayers) {
        int propertyCount = 0;
        if (gameBoard) {
            for (const auto& tilePtr : gameBoard->getTiles()) {
                Property* prop = dynamic_cast<Property*>(tilePtr.get());
                if (!prop) continue;
                if (prop->getOwner() == candidate->getUsername()) {
                    propertyCount++;
                }
            }
        }
        if (propertyCount > maxPropertyCount) {
            maxPropertyCount = propertyCount;
            propertyLeaders.clear();
            propertyLeaders.push_back(candidate);
        } else if (propertyCount == maxPropertyCount) {
            propertyLeaders.push_back(candidate);
        }
    }
    if (propertyLeaders.size() <= 1) return propertyLeaders;

    // Tie-break #2: jumlah kartu kemampuan
    std::vector<Player*> cardLeaders;
    int maxCardCount = -1;
    for (Player* candidate : propertyLeaders) {
        int cardCount = static_cast<int>(candidate->getHand().size());
        if (cardCount > maxCardCount) {
            maxCardCount = cardCount;
            cardLeaders.clear();
            cardLeaders.push_back(candidate);
        } else if (cardCount == maxCardCount) {
            cardLeaders.push_back(candidate);
        }
    }

    // Tie-break #3: jika masih seri, semua pemenang bersama.
    return cardLeaders;
}
