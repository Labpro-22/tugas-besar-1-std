#include "../include/models/GoToJailTile.hpp"
#include "../include/models/Player.hpp"

GoToJailTile::GoToJailTile(int position, const std::string& name, const std::string& code, const std::string& color)
    : SpecialTile(position, name, code, color, SpecialType::GO_TO_JAIL) {
}

void GoToJailTile::sendToJail(Player* player) {
    if (player == nullptr) {
        return;
    }

    
    player->setPosition(10);
    player->setStatus(PlayerStatus::JAILED);
}

void GoToJailTile::executeSpecial(Player* player, GameContext* ctx) {
    (void)ctx;
    if (player != nullptr) {
        sendToJail(player);
    }
}
