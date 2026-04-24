#include "../include/models/TaxTile.hpp"
#include "../include/models/Player.hpp"

TaxTile::TaxTile(int position, const std::string& name, const std::string& code, const std::string& color,
                 TaxType taxType, int flatAmount, double percentageRate)
    : ActionTile(position, name, code, color, ActionType::TAX),
      taxType(taxType),
      flatAmount(flatAmount),
      percentageRate(percentageRate) {
}

TaxType TaxTile::getTaxType() const {
    return taxType;
}

int TaxTile::getFlatAmount() const {
    return flatAmount;
}

double TaxTile::getPercentageRate() const {
    return percentageRate;
}

void TaxTile::setTaxType(TaxType type) {
    taxType = type;
}

void TaxTile::setFlatAmount(int amount) {
    if (amount >= 0) {
        flatAmount = amount;
    }
}

void TaxTile::setPercentageRate(double rate) {
    if (rate >= 0.0 && rate <= 1.0) {
        percentageRate = rate;
    }
}

int TaxTile::calculateTax(Player* player) const {
    if (player == nullptr) {
        return 0;
    }

    int tax = 0;

    if (flatAmount > 0) {
        tax = flatAmount;
    } else if (percentageRate > 0.0) {
        tax = static_cast<int>(player->getMoney() * percentageRate);
    }

    return tax;
}

void TaxTile::collectTax(Player* player) {
    if (player == nullptr) {
        return;
    }

    int tax = calculateTax(player);
    if (tax > 0 && player->getMoney() >= tax) {
        player->operator-=(tax);
    }
}

void TaxTile::executeAction(Player* player, GameContext* ctx) {
    (void)ctx;
    if (player != nullptr) {
        collectTax(player);
    }
}
