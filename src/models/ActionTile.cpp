#include "../include/models/ActionTile.hpp"


ActionTile::ActionTile(int position, const std::string& name, const std::string& code, const std::string& color,
                       ActionType actionType)
    : Tile(position, name, code, color), actionType(actionType) {
}

ActionType ActionTile::getActionType() const {
    return actionType;
}

void ActionTile::setActionType(ActionType type) {
    actionType = type;
}

void ActionTile::onLand(Player* player, GameContext* ctx) {
    if (player != nullptr) {
        executeAction(player, ctx);
    }
}


void ActionTile::onPass(Player* player) {
    (void)player;
    // Default implementation does nothing when passing
}
