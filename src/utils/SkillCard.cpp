#include "../../include/utils/SkillCard.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/core/GameContext.hpp"
#include "../../include/core/MovementHandler.hpp"
#include "../../include/models/GameBoard.hpp"
#include "../../include/models/Street.hpp"
#include "../../include/models/BuildingManager.hpp"
#include "../../include/views/GameView.hpp"
#include "../../include/data/TransactionLogger.hpp"

SkillCard::SkillCard(const string& type, int val, int duration)
    : cardType(type), value(val), remainingDuration(duration) {}

void SkillCard::decrementDuration() {
    if (remainingDuration > 0) remainingDuration--;
}

MoveCard::MoveCard(int steps)
    : SkillCard("MoveCard", steps, 0) {}

void MoveCard::activate(Player* player, GameContext* ctx) {
    if (!player || !ctx) return;

    if (player->getStatus() == JAILED) {
        string msg = "[MoveCard] Tidak dapat digunakan saat berada di Penjara.";
        if (ctx->gameView) ctx->gameView->showMessage(msg);
        else               cout << msg << "\n";
        return;
    }

    if (!ctx->hasMovementHandler()) return;

    ctx->movementHandler->movePlayer(player, value);
}

string MoveCard::getDescription() const {
    return "Maju " + to_string(value) + " langkah";
}

DiscountCard::DiscountCard(int percent)
    : SkillCard("DiscountCard", percent, 1) {}

void DiscountCard::activate(Player* player, GameContext* ctx) {
    (void)ctx;
    if (player) player->setDiscount(value);
}

string DiscountCard::getDescription() const {
    return "Diskon " + to_string(value) + "% untuk pembelian properti giliran ini";
}

// bisa dipakai saat JAILED untuk menghindari denda paksa

ShieldCard::ShieldCard()
    : SkillCard("ShieldCard", 0, 1) {}

void ShieldCard::activate(Player* player, GameContext* ctx) {
    (void)ctx;
    if (player) player->activateShield();
}

string ShieldCard::getDescription() const {
    return "Kebal terhadap satu tagihan atau sanksi selama giliran ini";
}

TeleportCard::TeleportCard()
    : SkillCard("TeleportCard", 0, 0) {}

void TeleportCard::activate(Player* player, GameContext* ctx) {
    if (!player || !ctx) return;

    // Guard: tidak bisa dipakai saat JAILED
    if (player->getStatus() == JAILED) {
        string msg = "[TeleportCard] Tidak dapat digunakan saat berada di Penjara.";
        if (ctx->gameView) ctx->gameView->showMessage(msg);
        else               cout << msg << "\n";
        return;
    }

    if (!ctx->hasMovementHandler()) return;

    int boardSize = ctx->getBoardSize();

    // Tampilkan semua tile sebagai referensi pilihan
    if (ctx->gameView) {
        ctx->gameView->showMessage("=== TELEPORT: Pilih petak tujuan ===");
        if (ctx->hasBoard()) {
            const auto& tiles = ctx->board->getTiles();
            for (int i = 0; i < static_cast<int>(tiles.size()); i++) {
                if (tiles[i]) {
                    ctx->gameView->showMessage(
                        to_string(i) + ". [" + tiles[i]->getCode() + "] " + tiles[i]->getName()
                    );
                }
            }
        }
        ctx->gameView->showMessage(
            "Masukkan nomor petak tujuan (0-" + to_string(boardSize - 1) + "):"
        );
    } else {
        cout << "=== TELEPORT: Masukkan nomor petak tujuan (0-" << boardSize - 1 << "): ";
    }

    int target = -1;
    if (!(cin >> target)) {
        cin.clear();
        cin.ignore(10000, '\n');
        if (ctx->gameView) ctx->gameView->showError("Input tidak valid. TeleportCard dibatalkan.");
        else               cout << "Input tidak valid. TeleportCard dibatalkan.\n";
        return;
    }
    cin.ignore();

    if (target < 0 || target >= boardSize) {
        if (ctx->gameView) ctx->gameView->showError("Nomor petak tidak valid. TeleportCard dibatalkan.");
        else               cout << "Nomor petak tidak valid. TeleportCard dibatalkan.\n";
        return;
    }

    ctx->movementHandler->teleportPlayer(player, target);
}

string TeleportCard::getDescription() const {
    return "Pindah ke petak manapun di papan permainan";
}

LassoCard::LassoCard()
    : SkillCard("LassoCard", 0, 0) {}

void LassoCard::activate(Player* player, GameContext* ctx) {
    if (!player || !ctx) return;
    if (!ctx->hasMovementHandler()) return;

    int boardSize = ctx->getBoardSize();
    int myPos     = player->getPosition();

    // Kumpulkan semua pemain aktif yang posisinya di depan 
    vector<Player*> candidates;
    for (Player* other : ctx->allPlayers) {
        if (!other)                         continue;
        if (other == player)                continue;
        if (other->getStatus() == BANKRUPT) continue;
        if (other->getStatus() == JAILED)   continue; 

        int dist = (other->getPosition() - myPos + boardSize) % boardSize;
        if (dist > 0) candidates.push_back(other);
    }

    if (candidates.empty()) {
        string msg = "Tidak ada pemain lawan yang dapat ditarik (tidak ada yang di depan atau semua di penjara).";
        if (ctx->gameView) ctx->gameView->showMessage(msg);
        else               cout << msg << "\n";
        return;
    }

    // Tampilkan kandidat
    if (ctx->gameView) {
        ctx->gameView->showMessage("=== LASSO: Pilih pemain yang ingin ditarik ===");
        for (int i = 0; i < static_cast<int>(candidates.size()); i++) {
            int dist = (candidates[i]->getPosition() - myPos + boardSize) % boardSize;
            ctx->gameView->showMessage(
                to_string(i + 1) + ". " + candidates[i]->getUsername() +
                " (petak " + to_string(candidates[i]->getPosition()) +
                ", " + to_string(dist) + " langkah di depan)"
            );
        }
        ctx->gameView->showMessage(
            "Pilih nomor pemain (1-" + to_string(candidates.size()) + ", 0 untuk batal):"
        );
    } else {
        cout << "=== LASSO: Pilih pemain (1-" << candidates.size() << ", 0 batal): ";
    }

    int choice = 0;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(10000, '\n');
        if (ctx->gameView) ctx->gameView->showMessage("Input tidak valid. LassoCard dibatalkan.");
        else               cout << "Input tidak valid. LassoCard dibatalkan.\n";
        return;
    }
    cin.ignore();

    if (choice <= 0 || choice > static_cast<int>(candidates.size())) {
        if (ctx->gameView) ctx->gameView->showMessage("LassoCard dibatalkan.");
        return;
    }

    Player* target = candidates[choice - 1];

    string msg = target->getUsername() + " ditarik ke posisi " +
                 player->getUsername() + " (petak " + to_string(myPos) + ").";
    if (ctx->gameView) ctx->gameView->showMessage(msg);
    else               cout << msg << "\n";

    // Pindahkan dan trigger onLand di posisi kita
    ctx->movementHandler->pullPlayer(target, myPos);
}

string LassoCard::getDescription() const {
    return "Menarik pemain lawan di depan ke posisi kamu";
}


DemolitionCard::DemolitionCard()
    : SkillCard("DemolitionCard", 0, 0) {}

void DemolitionCard::activate(Player* player, GameContext* ctx) {
    if (!player || !ctx) return;
    if (!ctx->hasBoard()) return;

    // Kumpulkan semua Street milik lawan yang punya bangunan (> 0 atau "H").

    vector<Street*> targets;
    const auto& tiles = ctx->board->getTiles();

    for (const auto& tilePtr : tiles) {
        Street* street = dynamic_cast<Street*>(tilePtr.get());
        if (!street) continue;
        if (street->getOwner().empty() || street->getOwner() == "BANK") continue;
        if (street->getOwner() == player->getUsername()) continue; // skip milik sendiri

        string bc = street->getBuildingCount();
        // Ada bangunan jika bukan "0" dan tidak kosong
        if (!bc.empty() && bc != "0") {
            targets.push_back(street);
        }
    }

    if (targets.empty()) {
        string msg = "Tidak ada properti lawan yang memiliki bangunan.";
        if (ctx->gameView) ctx->gameView->showMessage(msg);
        else               cout << msg << "\n";
        return;
    }

    // Tampilkan pilihan
    if (ctx->gameView) {
        ctx->gameView->showMessage("=== DEMOLITION: Pilih properti lawan yang ingin dihancurkan ===");
        for (int i = 0; i < static_cast<int>(targets.size()); i++) {
            string statusStr = (targets[i]->getStatus() == MORTGAGED) ? " [MORTGAGED]" : "";
            ctx->gameView->showMessage(
                to_string(i + 1) + ". " + targets[i]->getName() +
                " (" + targets[i]->getCode() + ")" +
                " milik " + targets[i]->getOwner() +
                " [bangunan: " + targets[i]->getBuildingCount() + "]" + statusStr
            );
        }
        ctx->gameView->showMessage(
            "Pilih nomor properti (1-" + to_string(targets.size()) + ", 0 untuk batal):"
        );
    } else {
        cout << "=== DEMOLITION: Pilih properti (1-" << targets.size() << ", 0 batal): ";
    }

    int choice = 0;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(10000, '\n');
        if (ctx->gameView) ctx->gameView->showMessage("Input tidak valid. DemolitionCard dibatalkan.");
        else               cout << "Input tidak valid. DemolitionCard dibatalkan.\n";
        return;
    }
    cin.ignore();

    if (choice <= 0 || choice > static_cast<int>(targets.size())) {
        if (ctx->gameView) ctx->gameView->showMessage("DemolitionCard dibatalkan.");
        return;
    }

    Street* chosen = targets[choice - 1];
    string buildingBefore = chosen->getBuildingCount();

    chosen->setBuildingCount("0");

    string msg = "DEMOLITION! Semua bangunan di " +
                 chosen->getName() + " (" + chosen->getCode() + ")" +
                 " milik " + chosen->getOwner() + " telah dihancurkan!" +
                 " [sebelum: " + buildingBefore + " → sekarang: 0]";
    if (ctx->gameView) ctx->gameView->showMessage(msg);
    else               cout << msg << "\n";

    // Log ke logger 
    if (ctx->hasLogger()) {
        ctx->logger->log(0, player->getUsername(), "DEMOLITION",
                         chosen->getName() + " (" + chosen->getCode() + ")" +
                         " milik " + chosen->getOwner() +
                         ": " + buildingBefore + " → 0 (semua dihancurkan)");
    }
}

string DemolitionCard::getDescription() const {
    return "Menghancurkan semua bangunan di satu properti milik pemain lawan (tanpa refund)";
}