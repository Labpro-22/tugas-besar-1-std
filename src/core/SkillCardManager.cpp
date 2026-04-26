#include "../../include/core/SkillCardManager.hpp"
#include "../../include/utils/SkillCard.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/core/GameContext.hpp"
#include "../../include/utils/CardDeck.hpp"


#include <cstdlib>

SkillCardManager::SkillCardManager(int maxSize) {
    maxHandSize = (maxSize > 0) ? static_cast<size_t>(maxSize) : static_cast<size_t>(0);
}

void SkillCardManager::initDeck() {
    // Stok sesuai spec
    for (int i = 0; i < 4; i++) 
        skillDeck.addCard(new MoveCard(rand() % 12 + 1));
    for (int i = 0; i < 3; i++) 
        skillDeck.addCard(new DiscountCard(rand() % 100 + 1));
    for (int i = 0; i < 2; i++) 
        skillDeck.addCard(new ShieldCard());
    for (int i = 0; i < 2; i++) 
        skillDeck.addCard(new TeleportCard(rand() % 40));
    for (int i = 0; i < 2; i++) 
        skillDeck.addCard(new LassoCard());
    for (int i = 0; i < 2; i++) 
        skillDeck.addCard(new DemolitionCard());

    skillDeck.shuffle();
}

bool SkillCardManager::isValidIndex(Player* player, int idx) {
    if (player == nullptr) {
        return false;
    }

    return idx >= 0 && idx < static_cast<int>(player->getHand().size());
}

void SkillCardManager::distributeCardToAll(vector<Player*> players) {
    for (Player* player : players) {
        distributeCardTo(player);
    }
}

SkillCard* SkillCardManager::distributeCardTo(Player* player) {
    if (!player) return nullptr;

    // drawTop() otomatis reshuffle kalau deck kosong
    SkillCard* card = skillDeck.drawTop();
    if (!card) return nullptr;

    player->receiveCard(card);

    // Return card kalau overflow (hand penuh), sinyal ke caller untuk drop
    if (player->getHand().size() > maxHandSize) {
        return card;
    }

    return nullptr;
}

void SkillCardManager::useCard(Player* player, int idx, GameContext* ctx) {
    if (!player || !ctx) return;
    if (player->hasUsedCard() || player->hasRolled()) return;
    if (player->getStatus() == JAILED) return;
    if (!isValidIndex(player, idx)) return;

    SkillCard* card = player->removeCard(idx);
    if (!card) return;

    card->activate(player, ctx);
    player->markCardUsed();

    skillDeck.discard(card); // masuk discard, bukan delete
}

void SkillCardManager::dropCard(Player* player, int idx) {
    if (!isValidIndex(player, idx)) return;
    SkillCard* card = player->removeCard(idx);
    if (card) skillDeck.discard(card);
}

void SkillCardManager::decrementDurations(Player* player) {
    if (player == nullptr) {
        return;
    }

    for (auto c : player->getHand()) {
        if (c != nullptr) {
            c->decrementDuration();
        }
    }
}

