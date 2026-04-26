#pragma once

#include "ActionTile.hpp"

// Forward declaration
class Property;
class Player;

class FestivalTile : public ActionTile {
public:
    FestivalTile(int position, const std::string& name, const std::string& code, const std::string& color);
    ~FestivalTile() = default;

    void applyFestivalEffect(Player* player, Property* property);
    void updateDuration();

    void executeAction(Player* player, GameContext* ctx = nullptr) override;
};
