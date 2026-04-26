#pragma once

#include "ActionTile.hpp"


enum TaxType {
    PPH,
    PBM
};

class TaxTile : public ActionTile {
private:
    TaxType taxType;
    int flatAmount;
    double percentageRate;

public:
    TaxTile(int position, const std::string& name, const std::string& code, const std::string& color,
            TaxType taxType, int flatAmount = 0, double percentageRate = 0.0);
    ~TaxTile() = default;

    TaxType getTaxType() const;
    int getFlatAmount() const;
    double getPercentageRate() const;

    void setTaxType(TaxType type);
    void setFlatAmount(int amount);
    void setPercentageRate(double rate);

    int calculateTax(Player* player) const;
    void collectTax(Player* player);

    void executeAction(Player* player, GameContext* ctx = nullptr) override;
};
