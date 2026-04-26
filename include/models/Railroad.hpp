#pragma once

#include "Property.hpp"
#include <map>

class Railroad : public Property {
private:
    std::map<int, int> rentTable;

public:
    Railroad(int position, const std::string& name, const std::string& code, const std::string& color,
             int purchasePrice, int mortgageValue);
    ~Railroad() = default;

    int getRent(int railroadCount) const;
    void setRentTable(const std::map<int, int>& table);

    int calculateRent(Player* visitor) override;
    void mortgage() override;
    void redeem() override;

    void onLand(Player* player, GameContext* ctx = nullptr) override;
    void onPass(Player* player) override;
};
