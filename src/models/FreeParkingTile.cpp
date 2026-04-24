#include "../include/models/FreeParkingTile.hpp"
#include "../include/models/Player.hpp"

FreeParkingTile::FreeParkingTile(int position, const std::string& name, const std::string& code, const std::string& color)
    : SpecialTile(position, name, code, color, SpecialType::FREE_PARKING) {
}

void FreeParkingTile::rest(Player* player) {
    if (player != nullptr) {
        // Player simply rests at free parking (no action taken)
        // This is a safe space where nothing happens
    }
}

void FreeParkingTile::executeSpecial(Player* player, GameContext* ctx) {
    (void)ctx;
    if (player != nullptr) {
        rest(player);
    }
}
