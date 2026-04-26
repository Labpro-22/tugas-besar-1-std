#pragma once

#include "Property.hpp"
#include <map>

class Utility : public Property {
private:
    std::map<int, int> multiplierTable;

public:
    Utility(int position, const std::string& name, const std::string& code, const std::string& color,
            int purchasePrice, int mortgageValue);
    ~Utility() = default;

    int getRent(int diceTotal, int utilityCount) const;
    void setMultiplierTable(const std::map<int, int>& table);

    int calculateRent(Player* visitor) override;
    void mortgage() override;
    void redeem() override;

    void onLand(Player* player, GameContext* ctx = nullptr) override;
    void onPass(Player* player) override;
};
