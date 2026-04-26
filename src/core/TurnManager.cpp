#include "../../include/core/TurnManager.hpp"
#include "../../include/models/Player.hpp"

namespace {
int findNextActiveIndex(const vector<Player*>& players, int startIndex) {
    if (players.empty()) {
        return -1;
    }

    int size = static_cast<int>(players.size());
    for (int offset = 1; offset <= size; ++offset) {
        int nextIndex = (startIndex + offset) % size;
        Player* candidate = players[nextIndex];
        if (candidate != nullptr && candidate->getStatus() != BANKRUPT) {
            return nextIndex;
        }
    }

    return -1;
}
}

TurnManager::TurnManager(vector<Player*> p, int maxT) {
    players = p;
    turnOrder.clear();
    for (Player* player : players) {
        if (player != nullptr) {
            turnOrder.push_back(player->getUsername());
        }
    }
    currentIndex = 0;
    currentTurnNumber = 1;
    maxTurn = maxT;
}

Player* TurnManager::getCurrentPlayer() {
    if (players.empty()) return nullptr;

    if (currentIndex >= 0 && currentIndex < static_cast<int>(players.size())) {
        Player* current = players[currentIndex];
        if (current != nullptr && current->getStatus() != BANKRUPT) {
            return current;
        }
    }

    int nextIndex = findNextActiveIndex(players, currentIndex);
    if (nextIndex >= 0) {
        currentIndex = nextIndex;
        return players[currentIndex];
    }

    return nullptr; // semua bankrupt
}

void TurnManager::advanceToNextPlayer() {
    if (players.empty()) return;

    int nextIndex = findNextActiveIndex(players, currentIndex);
    if (nextIndex < 0) {
        return;
    }

    if (nextIndex <= currentIndex) {
        currentTurnNumber++;
    }

    currentIndex = nextIndex;
}

int TurnManager::getCurrentTurnNumber() { return currentTurnNumber; }
int TurnManager::getMaxTurn() { return maxTurn; }

bool TurnManager::isMaxTurnReached() {
    return currentTurnNumber > maxTurn;
}

vector<string> TurnManager::getTurnOrder() {
    if (turnOrder.empty()) {
        vector<string> order;
        for (Player* player : players) {
            if (player != nullptr) {
                order.push_back(player->getUsername());
            }
        }
        return order;
    }

    return turnOrder;
}

void TurnManager::setTurnOrder(vector<string> order) {
    turnOrder = order;

    vector<Player*> reordered;
    reordered.reserve(players.size());

    for (const string& username : turnOrder) {
        for (Player* player : players) {
            if (player != nullptr && player->getUsername() == username) {
                reordered.push_back(player);
                break;
            }
        }
    }

    for (Player* player : players) {
        bool alreadyAdded = false;
        for (Player* reorderedPlayer : reordered) {
            if (reorderedPlayer == player) {
                alreadyAdded = true;
                break;
            }
        }

        if (!alreadyAdded && player != nullptr) {
            reordered.push_back(player);
        }
    }

    if (!reordered.empty()) {
        players = reordered;
        currentIndex = 0;
    }
}

void TurnManager::resetTurnFlags(Player* p) {
    if (p == nullptr) {
        return;
    }

    p->resetTurnFlags();
}
