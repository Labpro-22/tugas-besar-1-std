#pragma once

#include "ActionTile.hpp"
#include "../utils/CardDeck.hpp"
#include "../include/utils/ActionCard.hpp"
#include <vector>

// Forward declaration
class ActionCard;

class CommunityChestTile : public ActionTile {
private:
    CardDeck<ActionCard>* deck;

public:
    CommunityChestTile(int position, const std::string& name, const std::string& code, const std::string& color);
    ~CommunityChestTile() = default;

    void setDeck(CardDeck<ActionCard>* d);
    CardDeck<ActionCard>* getDeck() const;

    ActionCard* drawCard(Player* player, CardDeck<ActionCard>* deck);

    void executeAction(Player* player, GameContext* ctx = nullptr) override;
};
