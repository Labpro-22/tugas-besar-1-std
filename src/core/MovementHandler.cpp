#include "../../include/core/MovementHandler.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/utils/Dice.hpp"
#include "../../include/models/Tile.hpp"
#include "../../include/models/GameBoard.hpp"
#include "../../include/models/JailTile.hpp"
#include "../../include/models/GoTile.hpp"
#include "../../include/core/GameContext.hpp"
#include "../../include/data/TransactionLogger.hpp"

using namespace std;

namespace {

JailTile* findJailTile(GameBoard* board) {
    if (!board) return nullptr;
    const auto& tiles = board->getTiles();
    for (const auto& t : tiles) {
        JailTile* jt = dynamic_cast<JailTile*>(t.get());
        if (jt) return jt;
    }
    return nullptr;
}

GoTile* findGoTile(GameBoard* board) {
    if (!board) return nullptr;
    Tile* t = board->getTileAt(0);
    return dynamic_cast<GoTile*>(t);
}

} 

MovementHandler::MovementHandler(GameBoard* b, GameContext* c)
    : board(b), ctx(c),
      boardSize(0),
      goSalary(0),
      jailFine(0) {
    if (board && !board->getTiles().empty()) {
        boardSize = static_cast<int>(board->getTiles().size());
    }
    
}


// Ambil boardSize aktual dari board 
static int getBoardSz(GameBoard* board, GameContext* ctx) {
    if (board && !board->getTiles().empty())
        return static_cast<int>(board->getTiles().size());
    if (ctx) return ctx->getBoardSize();
    return 40;
}

// Ambil jailFine dari JailTile di board
static int getJailFineSz(GameBoard* board) {
    if (!board) return 0;
    for (const auto& t : board->getTiles()) {
        JailTile* jt = dynamic_cast<JailTile*>(t.get());
        if (jt) return jt->getFineAmount();
    }
    return 0;
}

//   saat consecutiveDoubles mencapai 3, player langsung ke penjara (gak dapet gaji)
//   Caller  harus udah increment consecutiveDoubles sebelum memanggil movePlayer jika dadu double.
void MovementHandler::movePlayer(Player* player, int steps) {
    if (!player) return;
    if (player->getStatus() == BANKRUPT) return;
    if (player->getStatus() == JAILED) return; // jailed dihandle handleJailTurn

    //  double ke 3 langsung penjara, tidak dapat gaji GO, giliran berakhir.
    if (player->getConsecutiveDoubles() >= 3) {
        sendToJail(player); // reset consecutiveDoubles di sendToJail
        // Log 
        if (ctx && ctx->hasLogger()) {
            ctx->logger->log(0, player->getUsername(), "PENJARA",
                             "Melanggar batas kecepatan (double 3x) — langsung ke penjara");
        }
        return;
    }

    int sz = getBoardSz(board, ctx);
    int oldPos = player->getPosition();
    int rawNewPos = oldPos + steps;
    int newPos = rawNewPos % sz;
    if (newPos < 0) newPos += sz;

    // Lewati GO -> dapet gaji
    if (rawNewPos >= sz) {
        handlePassGo(player);
    }

    player->setPosition(newPos);

    // Trigger onLand 
    if (board) {
        Tile* tile = board->getTileAt(newPos);
        if (tile) tile->onLand(player, ctx);
    }
}



// Tidak bisa dipakai saat JAILED 

void MovementHandler::teleportPlayer(Player* player, int target) {
    if (!player) return;
    if (player->getStatus() == BANKRUPT) return;
    if (player->getStatus() == JAILED) return; // guard ekstra

    int sz = getBoardSz(board, ctx);
    int oldPos = player->getPosition();
    int normalizedTarget = ((target % sz) + sz) % sz;

    // Beri gaji hanya jika benar-benar wrap-around melewati GO (target raw > oldPos via modulo)
    if (target >= sz || (target >= 0 && normalizedTarget < oldPos)) {
        handlePassGo(player);
    }

    player->setPosition(normalizedTarget);

    if (board) {
        Tile* tile = board->getTileAt(normalizedTarget);
        if (tile) tile->onLand(player, ctx);
    }
}

// Tidak trigger passGo 
// Target JAILED tidak bisa ditarik 
void MovementHandler::pullPlayer(Player* target, int toPosition) {
    if (!target) return;
    if (target->getStatus() == BANKRUPT) return;

    int sz = getBoardSz(board, ctx);
    int normalizedTarget = ((toPosition % sz) + sz) % sz;

    target->setPosition(normalizedTarget);

    // Trigger tile effect 
    if (board) {
        Tile* tile = board->getTileAt(normalizedTarget);
        if (tile) tile->onLand(target, ctx);
    }
}

// berikan gaji saat lewat/berhenti di GO. Dipanggil oleh movePlayer dan teleportPlayer
// gak dipanggil oleh sendToJail, tidak dapat gaji, dan GoToJail juga tidak)
void MovementHandler::handlePassGo(Player* player) {
    if (!player) return;

    GoTile* goTile = findGoTile(board);
    if (goTile) {
        goTile->onPass(player);
        if (ctx && ctx->hasLogger()) {
            ctx->logger->log(0, player->getUsername(), "GAJI_GO",
                             "Mendapat gaji M" + to_string(goTile->getSalary()) + " melewati GO");
        }
        return;
    }

    // Fallback 
    (*player) += goSalary;
}

// pindahkan player ke petak penjara dan set status JAILED,Reset consecutiveDoubles dan jailTurnsRemaining
// Dipanggil oleh: GoToJailTile::executeSpecial, movePlayer(double ke-3),ActionCard "Masuk Penjara"
void MovementHandler::sendToJail(Player* player) {
    if (!player) return;

    player->setStatus(JAILED);
    player->resetConsecutiveDoubles();
    player->resetJailTurns();

    JailTile* jailTile = findJailTile(board);
    if (jailTile) {
        player->setPosition(jailTile->getPosition());
        return;
    }

    // Fallback: JailTile ada di posisi 10 sesuai spek papan
    player->setPosition(10);
}


// handleJailTurn — kelola giliran saat player berstatus JAILED.
// Caller (CommandProcessor)  bertanggung jawab:
//   - Menampilkan pilihan ke player (bayar / kartu / coba dadu)
//   - Jika FORCED_OUT atau ESCAPED_DOUBLE: izinkan roll dadu lagi, (movePlayer normal) untuk giliran tersebut.
//   - Jika STILL_JAILED: giliran berakhir, tidak ada gerakan.

JailResult MovementHandler::handleJailTurn(Player* player, Dice& dice) {
    if (!player) return STILL_JAILED;

    int turnsUsed = player->getJailTurnsRemaining();

    // Giliran ke 4 bayar denda
    if (turnsUsed >= 3) {
        int fine = getJailFineSz(board);
        // Jika tidak mampu bayar, caller harus handle kebangkrutan.
        (*player) -= fine;
        player->setStatus(ACTIVE);
        player->resetJailTurns();
        if (ctx && ctx->hasLogger()) {
            ctx->logger->log(0, player->getUsername(), "PENJARA",
                             "Giliran ke-4: wajib bayar denda M" + to_string(fine) + " keluar penjara");
        }
        return FORCED_OUT;
    }

    // Coba keluar 
    if (dice.isDouble()) {
        player->setStatus(ACTIVE);
        player->resetJailTurns();
        player->resetConsecutiveDoubles();
        if (ctx && ctx->hasLogger()) {
            ctx->logger->log(0, player->getUsername(), "PENJARA",
                             "Keluar penjara dengan double — lanjut giliran");
        }
        return ESCAPED_DOUBLE;
    }

    // Gagal keluar, increment hitung giliran di penjara
    player->incrementJailTurns();
    if (ctx && ctx->hasLogger()) {
        ctx->logger->log(0, player->getUsername(), "PENJARA",
                         "Gagal keluar penjara (giliran " +
                         to_string(player->getJailTurnsRemaining()) + "/3)");
    }
    return STILL_JAILED;
}