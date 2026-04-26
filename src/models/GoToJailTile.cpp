#include "../include/models/GoToJailTile.hpp"
#include "../include/models/Player.hpp"
#include "../include/core/GameContext.hpp"
#include "../include/core/MovementHandler.hpp"

GoToJailTile::GoToJailTile(int position, const std::string& name, const std::string& code, const std::string& color)
    : SpecialTile(position, name, code, color, SpecialType::GO_TO_JAIL) {
}

void GoToJailTile::sendToJail(Player* player) {
    if (player == nullptr) return;
    // Reset semua state penjara dengan benar
    player->setStatus(PlayerStatus::JAILED);
    player->resetConsecutiveDoubles();
    player->resetJailTurns();
    player->setPosition(10);
}

void GoToJailTile::executeSpecial(Player* player, GameContext* ctx) {
    if (player == nullptr) return;
    // Delegasikan ke MovementHandler jika tersedia agar logika terpusat
    if (ctx && ctx->hasMovementHandler()) {
        ctx->movementHandler->sendToJail(player);
    } else {
        sendToJail(player);
    }
}
