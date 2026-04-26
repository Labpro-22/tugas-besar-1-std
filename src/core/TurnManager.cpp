#include "../../include/core/TurnManager.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/models/GameBoard.hpp"

using namespace std;


// Helper cari index pemain aktif berikutnya 

namespace {
int findNextActiveIndex(const vector<Player*>& players, int startIndex) {
    if (players.empty()) return -1;
    int size = static_cast<int>(players.size());
    for (int offset = 1; offset <= size; ++offset) {
        int idx = (startIndex + offset) % size;
        if (players[idx] && players[idx]->getStatus() != BANKRUPT) {
            return idx;
        }
    }
    return -1; 
}
} 

TurnManager::TurnManager(const vector<Player*>& p, int maxT)
    : players(p), currentIndex(0), currentTurnNumber(1), maxTurn(maxT) {
    for (Player* player : players) {
        if (player) turnOrder.push_back(player->getUsername());
    }
}

Player* TurnManager::getCurrentPlayer() {
    if (players.empty()) return nullptr;

    if (currentIndex >= 0 && currentIndex < static_cast<int>(players.size())) {
        Player* current = players[currentIndex];
        if (current && current->getStatus() != BANKRUPT) {
            return current;
        }
    }

    int nextIdx = findNextActiveIndex(players, currentIndex);
    if (nextIdx >= 0) {
        currentIndex = nextIdx;
        return players[currentIndex];
    }
    return nullptr;
}


// Increment currentTurnNumber ketika satu putaran penuh selesai
void TurnManager::advanceToNextPlayer() {
    if (players.empty()) return;

    int nextIdx = findNextActiveIndex(players, currentIndex);
    if (nextIdx < 0) return; // semua bankrupt

    // satu putaran selesai
    if (nextIdx <= currentIndex) {
        currentTurnNumber++;
    }

    currentIndex = nextIdx;
}

// Getters
int TurnManager::getCurrentTurnNumber() const { return currentTurnNumber; }
int TurnManager::getMaxTurn() const { return maxTurn; }

bool TurnManager::isMaxTurnReached() const {
    if (maxTurn <= 0) return false;          
    return currentTurnNumber > maxTurn;
}

vector<string> TurnManager::getTurnOrder() const {
    if (!turnOrder.empty()) return turnOrder;

    // Fallback
    vector<string> order;
    for (Player* p : players) {
        if (p) order.push_back(p->getUsername());
    }
    return order;
}

// susun ulang urutan giliran (untuk load game).
// Mengubah vector players agar urutannya sesuai order.
void TurnManager::setTurnOrder(const vector<string>& order) {
    turnOrder = order;

    vector<Player*> reordered;
    reordered.reserve(players.size());

    for (const string& username : order) {
        for (Player* p : players) {
            if (p && p->getUsername() == username) {
                reordered.push_back(p);
                break;
            }
        }
    }

    // Tambahkan player yang tidak ada di order (just incase)
    for (Player* p : players) {
        if (!p) continue;
        bool found = false;
        for (Player* r : reordered) {
            if (r == p) { found = true; break; }
        }
        if (!found) reordered.push_back(p);
    }

    if (!reordered.empty()) {
        players = reordered;
        currentIndex = 0;
    }
}

void TurnManager::resetTurnFlags(Player* p) {
    if (p) p->resetTurnFlags();
}

void TurnManager::setCurrentTurnNumber(int n) {
    if (n >= 1) currentTurnNumber = n;
}

int TurnManager::getActivePlayerCount() const {
    int count = 0;
    for (Player* p : players) {
        if (p && p->getStatus() != BANKRUPT) count++;
    }
    return count;
}

// sinkronkan currentPlayerIndex di GameBoard 
void TurnManager::syncWithBoard(GameBoard* board) const {
    if (!board) return;
    Player* current = players.empty() ? nullptr : players[currentIndex];
    if (current) {
        board->setCurrentPlayerByUsername(current->getUsername());
    }
}