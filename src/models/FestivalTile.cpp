#include "../include/models/FestivalTile.hpp"
#include "../include/models/Player.hpp"
#include "../include/models/Property.hpp"


FestivalTile::FestivalTile(int position, const std::string& name, const std::string& code, const std::string& color)
    : ActionTile(position, name, code, color, ActionType::FESTIVAL) {
}

void FestivalTile::applyFestivalEffect(Player* player, Property* property) {
    if (player == nullptr || property == nullptr) {
        return;
    }

    int currentMult = property->getFestivalMultiplier();
    if (currentMult < 8) {
        property->setFestivalMultiplier(currentMult * 2);
    }
    // Kalau max (8x) atau baru pertama (1x), tetap reset durasi
    property->setFestivalDuration(3);
}

void FestivalTile::updateDuration() {
    // Update logic for festival duration tracking
}

void FestivalTile::executeAction(Player* player, GameContext* ctx) {
    (void)ctx;
    if (player != nullptr) {
        // Apply festival effect to all properties owned by the player
        // This will be coordinated with the game controller
    }
}
