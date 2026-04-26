#include "../include/models/JailTile.hpp"
#include "../include/models/Player.hpp"

JailTile::JailTile(int position, const std::string& name, const std::string& code, const std::string& color, int fineAmount)
    : SpecialTile(position, name, code, color, SpecialType::JAIL), fineAmount(fineAmount) { }

int JailTile::getFineAmount() const {
    return fineAmount;
}

void JailTile::setFineAmount(int amount) {
    if (amount >= 0) {
        fineAmount = amount;
    }
}

void JailTile::sendToJail(Player* player) {
    if (player == nullptr) {
        return;
    }

    player->setStatus(PlayerStatus::JAILED);
    player->setPosition(10);
}

void JailTile::visitJail(Player* player) {
    (void)player;
    // Player hanya berkunjung (Just Visiting), tidak ada efek apapun.
    // incrementJailTurns hanya dilakukan oleh MovementHandler::handleJailTurn saat STILL_JAILED.
}

JailResult JailTile::attemptEscape(Player* player) {
    if (player == nullptr) {
        return JailResult::STILL_JAILED;
    }

    if (player->getMoney() < fineAmount) {
        // Tidak mampu bayar: caller harus handle kebangkrutan
        return JailResult::STILL_JAILED;
    }

    payFine(player);
    return JailResult::FORCED_OUT;
}

void JailTile::payFine(Player* player) {
    if (player == nullptr) {
        return;
    }

    if (player->getMoney() >= fineAmount) {
        player->operator-=(fineAmount);
        player->setStatus(PlayerStatus::ACTIVE);
        player->resetJailTurns();
    }
}

void JailTile::executeSpecial(Player* player, GameContext* ctx) {
    (void)ctx;
    if (player != nullptr) {
        visitJail(player);
    }
}
