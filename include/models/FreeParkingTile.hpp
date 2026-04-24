#pragma once

#include "SpecialTile.hpp"

class FreeParkingTile : public SpecialTile {
public:
    FreeParkingTile(int position, const std::string& name, const std::string& code, const std::string& color);
    ~FreeParkingTile() = default;

    void rest(Player* player);
    void executeSpecial(Player* player, GameContext* ctx = nullptr) override;
};
