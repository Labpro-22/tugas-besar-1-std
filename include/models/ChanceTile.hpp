#pragma once

#include "ActionTile.hpp"
#include "../utils/CardDeck.hpp"
#include "../include/utils/ActionCard.hpp"
#include "../include/utils/CardDeck.hpp"
#include "../include/models/Player.hpp"

// Forward declaration

class ChanceTile : public ActionTile {
private:
    CardDeck<ActionCard>* deck;

public:
    ChanceTile(int position, const std::string& name, const std::string& code, const std::string& color);
    ~ChanceTile() = default;

    void setDeck(CardDeck<ActionCard>* d);
    CardDeck<ActionCard>* getDeck() const;

    ActionCard* drawCard(Player* player, CardDeck<ActionCard>* deck);
    void executeAction(Player* player, GameContext* ctx = nullptr) override;

};
