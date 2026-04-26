#include "../include/models/SpecialTile.hpp"

SpecialTile::SpecialTile(int position, const std::string& name, const std::string& code, const std::string& color,
                           SpecialType specialType)
    : ActionTile(position, name, code, color, ActionType::SPECIAL), specialType(specialType) {
}

SpecialType SpecialTile::getSpecialType() const {
    return specialType;
}

void SpecialTile::setSpecialType(SpecialType type) {
    specialType = type;
}

void SpecialTile::executeAction(Player* player, GameContext* ctx) {
    if (player != nullptr) {
        executeSpecial(player, ctx);
    }
}

void SpecialTile::onPass(Player* player) {
    (void)player;
    // Default implementation does nothing when passing
}