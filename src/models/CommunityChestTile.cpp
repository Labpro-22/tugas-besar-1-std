#include "../include/models/CommunityChestTile.hpp"
#include "../include/utils/CardDeck.hpp"
#include "../include/core/GameContext.hpp"

CommunityChestTile::CommunityChestTile(int position, const std::string& name, const std::string& code, const std::string& color)
    : ActionTile(position, name, code, color, ActionType::COMMUNITY_CHEST), deck(nullptr) {
}

void CommunityChestTile::setDeck(CardDeck<ActionCard>* d) {
    deck = d;
}

CardDeck<ActionCard>* CommunityChestTile::getDeck() const {
    return deck;
}

ActionCard* CommunityChestTile::drawCard(Player* player, CardDeck<ActionCard>* d) {
    (void)player;
    if (d == nullptr) return nullptr;
    if (d->isEmpty()) return nullptr;
    return d->drawTop();
}

void CommunityChestTile::executeAction(Player* player, GameContext* ctx) {
    if (player == nullptr) return;

    ActionCard* card = drawCard(player, deck);
    if (card == nullptr) return;

    if (ctx != nullptr) {
        card->execute(player, ctx);
    }
    if (deck != nullptr) {
        deck->discard(card);
    }
}
