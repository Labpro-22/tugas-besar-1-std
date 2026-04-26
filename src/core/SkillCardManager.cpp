#include "SkillCardManager.hpp"
#include "SkillCard.hpp"
#include "Player.hpp"
#include "GameContext.hpp"
#include "CardDeck.hpp"

#include <cstdlib>
#include <stdexcept>

using namespace std;

SkillCardManager::SkillCardManager(int maxSize)
    : maxHandSize(maxSize > 0 ? maxSize : 3) {}

void SkillCardManager::initDeck() {
    for (int i = 0; i < 4; i++)
        skillDeck.addCard(new MoveCard(rand() % 12 + 1));

    for (int i = 0; i < 3; i++)
        skillDeck.addCard(new DiscountCard(rand() % 100 + 1));

    for (int i = 0; i < 2; i++)
        skillDeck.addCard(new ShieldCard());

    for (int i = 0; i < 2; i++)
        skillDeck.addCard(new TeleportCard());

    for (int i = 0; i < 2; i++)
        skillDeck.addCard(new LassoCard());

    for (int i = 0; i < 2; i++)
        skillDeck.addCard(new DemolitionCard());

    skillDeck.shuffle();
}

bool SkillCardManager::isValidIndex(Player* player, int idx) const {
    if (!player) return false;
    return idx >= 0 && idx < static_cast<int>(player->getHand().size());
}

bool SkillCardManager::isHandOverflow(Player* player) const {
    if (!player) return false;
    return static_cast<int>(player->getHand().size()) > maxHandSize;
}

void SkillCardManager::distributeCardToAll(const vector<Player*>& players) {
    for (Player* player : players) {
        if (player != nullptr && player->getStatus() != BANKRUPT) {
            distributeCardTo(player);
        }
    }
}


// overflow harus dicek oleh caller setelah return.
SkillCard* SkillCardManager::distributeCardTo(Player* player) {
    if (!player) return nullptr;

    SkillCard* card = skillDeck.drawTop();
    if (!card) return nullptr;

    player->receiveCard(card);
    return card;
}

// Guard: sebelum lempar dadu (hasRolled), Max 1 kartu per giliran (hasUsedCard)
// Kartu dibuang ke discard pile setelah dipakai
void SkillCardManager::useCard(Player* player, int idx, GameContext* ctx) {
    if (!player || !ctx) return;

    if (player->hasRolled()) return;

    if (player->hasUsedCard()) return;

    if (!isValidIndex(player, idx)) return;

    SkillCard* card = player->removeCard(idx);
    if (!card) return;

    card->activate(player, ctx);
    player->markCardUsed();
    skillDeck.discard(card);
}

void SkillCardManager::dropCard(Player* player, int idx) {
    if (!isValidIndex(player, idx)) return;
    SkillCard* card = player->removeCard(idx);
    if (card) skillDeck.discard(card);
}

// Kurangi sisa durasi semua kartu di hand player,Dipanggil di awal giliran player sblm distribusi kartu baru)

void SkillCardManager::decrementDurations(Player* player) {
    if (!player) return;

    for (SkillCard* c : player->getHand()) {
        if (c) c->decrementDuration();
    }

    // Bersihkan discount jika gak ada yg aktif
    bool hasActiveDiscount = false;
    for (SkillCard* c : player->getHand()) {
        if (c && c->getCardType() == "DiscountCard" && c->getRemainingDuration() > 0) {
            hasActiveDiscount = true;
            break;
        }
    }
    if (!hasActiveDiscount) {
        player->clearDiscount();
    }
}