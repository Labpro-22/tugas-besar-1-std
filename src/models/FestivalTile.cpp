#include "../include/models/FestivalTile.hpp"
#include "../include/models/Player.hpp"
#include "../include/models/Property.hpp"
#include "../include/core/GameContext.hpp"
#include "../include/models/GameBoard.hpp"


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
    if (player == nullptr || ctx == nullptr || !ctx->hasBoard()) return;

    const auto& tiles = ctx->board->getTiles();
    for (const auto& tilePtr : tiles) {
        Property* prop = dynamic_cast<Property*>(tilePtr.get());
        if (!prop) continue;
        if (prop->getOwner() != player->getUsername()) continue;
        if (prop->getStatus() == MORTGAGED) continue;
        applyFestivalEffect(player, prop);
    }
}
