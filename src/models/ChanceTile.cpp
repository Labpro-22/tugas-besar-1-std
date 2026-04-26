#include "../include/models/ChanceTile.hpp"
#include "../include/core/GameContext.hpp"

ChanceTile::ChanceTile(int position, const std::string& name, const std::string& code, const std::string& color)
    : ActionTile(position, name, code, color, ActionType::CHANCE), deck(nullptr) {
}

void ChanceTile::setDeck(CardDeck<ActionCard>* d) {
    deck = d;
}

CardDeck<ActionCard>* ChanceTile::getDeck() const {
    return deck;
}

ActionCard* ChanceTile::drawCard(Player* player, CardDeck<ActionCard>* d) {
    (void)player;
    if (d == nullptr) return nullptr;
    return d->drawTop();
}

void ChanceTile::executeAction(Player* player, GameContext* ctx) {
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

