#include "../include/models/Property.hpp"
#include "../include/models/Player.hpp"

Property::Property(int position, const std::string& name, const std::string& code, const std::string& color,
                   int purchasePrice, int mortgageValue)
    : Tile(position, name, code, color),
      owner(""),
      status(StatusType::BANK),
      festivalMultiplier(1),
      festivalDuration(0),
      purchasePrice(purchasePrice),
      mortgageValue(mortgageValue) {
}

std::string Property::getOwner() const {
    return owner;
}

StatusType Property::getStatus() const {
    return status;
}

int Property::getFestivalMultiplier() const {
    return festivalMultiplier;
}

int Property::getFestivalDuration() const {
    return festivalDuration;
}

int Property::getPurchasePrice() const {
    return purchasePrice;
}

int Property::getMortgageValue() const {
    return mortgageValue;
}

void Property::setOwner(const std::string& username) {
    owner = username;
}

void Property::setStatus(StatusType newStatus) {
    status = newStatus;
}

void Property::setFestivalMultiplier(int multiplier) {
    if (multiplier > 0) {
        festivalMultiplier = multiplier;
    }
}

void Property::setFestivalDuration(int duration) {
    if (duration >= 0) {
        festivalDuration = duration;
    }
}

void Property::mortgage() {
    if (status == StatusType::OWNED) {
        status = StatusType::MORTGAGED;
    }
}

void Property::redeem() {
    if (status == StatusType::MORTGAGED) {
        status = StatusType::OWNED;
    }
}

void Property::onLand(Player* player, GameContext* ctx) {
    (void)ctx;
    if (player == nullptr) return;

    if (status == StatusType::OWNED && owner != player->getUsername()) {
        int rent = calculateRent(player);
        (void)rent;
        // Rent collection will be handled by game controller
    }
}

void Property::onPass(Player* player) {
    // Default: no action when passing
    
}
