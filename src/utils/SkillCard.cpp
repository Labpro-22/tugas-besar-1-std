#include "../../include/utils/SkillCard.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/core/GameContext.hpp"
#include "../../include/core/MovementHandler.hpp"


SkillCard::SkillCard(string type, int val, int duration) {
    cardType = type;
    value = val;
    remainingDuration = duration;
}

void SkillCard::decrementDuration() {
    if (remainingDuration > 0) remainingDuration--;
}

string SkillCard::getCardType() const {
    return cardType;
}

int SkillCard::getValue() const {
    return value;
}

int SkillCard::getRemainingDuration() const {
    return remainingDuration;
}

void SkillCard::setRemainingDuration(int d) {
    remainingDuration = d;
}

// Turunan Skillcard

MoveCard::MoveCard(int steps)
    : SkillCard("MOVE", steps, 0) {}

void MoveCard::activate(Player* player, GameContext* ctx) {
    if (!player || !ctx) return;
    if (player->getStatus() == JAILED) return; // tambah guard

    if (ctx->movementHandler != nullptr) {
        ctx->movementHandler->movePlayer(player, value);
    }
}

string MoveCard::getDescription() {
    return "Maju " + to_string(value) + " langkah";
}

string MoveCard::getCardType() const { return "MoveCard"; }

ShieldCard::ShieldCard()
    : SkillCard("SHIELD", 0, 1) {}

void ShieldCard::activate(Player* player, GameContext* ctx) {
    (void)ctx;
    // TODO: efek akhir shield yg bergantung pada transaksi (Bank,Tax,Property) yang harus konsisten memakai operator-=
    //  cek isShieldActive() sebelum menagih biaya.
    if (player != nullptr) {
        player->activateShield();
    }
}

string ShieldCard::getDescription() {
    return "Menahan pembayaran berikutnya";
}

string ShieldCard::getCardType() const { return "ShieldCard"; }

TeleportCard::TeleportCard(int target)
    : SkillCard("TELEPORT", target, 0) {}

void TeleportCard::activate(Player* player, GameContext* ctx) {
    if (!player || !ctx || !ctx->movementHandler) return;
    if (player->getStatus() == JAILED) return;

    // TODO: value seharusnya dari input user saat kartu dipakai.
    // Reminder: teleport harus tetap trigger onLand() dan jika lintasan melewati GO,
    // MovementHandler::teleportPlayer wajib memberi gaji GO.
    ctx->movementHandler->teleportPlayer(player, value);
}

string TeleportCard::getDescription() {
    return "Teleport ke posisi " + to_string(value);
}

string TeleportCard::getCardType() const { return "TeleportCard"; }


DiscountCard::DiscountCard(int percent)
    : SkillCard("DISCOUNT", percent, 1) {}

void DiscountCard::activate(Player* player, GameContext* ctx) {
    (void)ctx;
    // TODO: masih harus membaca getDiscountPercent() saat ngitung harga agar efek diskon aktif.
    if (player != nullptr) {
        player->setDiscount(value);
    }
}

string DiscountCard::getDescription() {
    return "Diskon " + to_string(value) + "%";
}

string DiscountCard::getCardType() const { return "DiscountCard"; }

LassoCard::LassoCard()
    : SkillCard("LASSO", 0, 0) {}

void LassoCard::activate(Player* player, GameContext* ctx) {
    // TODO: implementasi pemilihan target player dari CLI.
    // Reminder: target tidak boleh JAILED dan definisi "di depan" pakai wrap-around board.
    // Reminder: setelah ditarik, target harus trigger efek onLand() via pullPlayer().
}

string LassoCard::getDescription() {
    return "Menarik pemain lain ke posisi kamu";
}

string LassoCard::getCardType() const { return "LassoCard"; }

DemolitionCard::DemolitionCard()
    : SkillCard("DEMOLITION", 0, 0) {}

void DemolitionCard::activate(Player* player, GameContext* ctx) {
    // TODO: tunggu implementasi BuildingManager trus hubungin ke logika penghancuran 1 bangunan milik lawan.
}

string DemolitionCard::getDescription() {
    return "Menghancurkan bangunan lawan";
}

string DemolitionCard::getCardType() const { return "DemolitionCard"; }
