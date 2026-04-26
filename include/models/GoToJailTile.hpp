#pragma once

#include "SpecialTile.hpp"

class GoToJailTile : public SpecialTile {
public:
    GoToJailTile(int position, const std::string& name, const std::string& code, const std::string& color);
    ~GoToJailTile() = default;

    void sendToJail(Player* player);
    void executeSpecial(Player* player, GameContext* ctx = nullptr) override;
};
