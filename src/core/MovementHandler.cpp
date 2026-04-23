#include "../../include/core/MovementHandler.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/utils/Dice.hpp"
#include "../../include/models/Tile.hpp"
#include "../../include/models/GameBoard.hpp"
#include "../../include/models/JailTile.hpp"

namespace {
JailTile* findJailTile(GameBoard* board, int boardSize) {
    if (board == nullptr) {
        return nullptr;
    }

    for (int i = 0; i < boardSize; i++) {
        JailTile* jailTile = dynamic_cast<JailTile*>(board->getTileAt(i));
        if (jailTile != nullptr) {
            return jailTile;
        }
    }

    return nullptr;
}
}

MovementHandler::MovementHandler(GameBoard* b, GameContext* ctx, int boardSizeValue, int goSalaryValue, int jailFineValue) : ctx(ctx) {
    board = b;
    // Placeholder buat tes sebelum ukuran board dibaca dari config/board object.
    boardSize = (boardSizeValue > 0) ? boardSizeValue : 40;
    // Placeholder buat tes sebelum salary GO dibaca dari special config.
    goSalary = (goSalaryValue >= 0) ? goSalaryValue : 200;
    jailFine = (jailFineValue >= 0) ? jailFineValue : 50;
}

void MovementHandler::movePlayer(Player* player, int steps) {
    if (!player) return;
    if (player->getStatus() == BANKRUPT) return;
    if (player->getStatus() == JAILED) return;

    int oldPos = player->getPosition();
    int rawNewPos = oldPos + steps;
    int newPos = rawNewPos % boardSize;
    if (newPos < 0) newPos += boardSize;

    // Handle pass GO
    if (rawNewPos >= boardSize) {
        handlePassGo(player);
        
    }

    player->setPosition(newPos);

    if (board != nullptr) {
        Tile* tile = board->getTileAt(newPos);
        if (tile != nullptr) {
            tile->onLand(player);
        }
    }
}

void MovementHandler::teleportPlayer(Player* player, int target) {
    if (!player) return;

    int oldPos = player->getPosition();
    int normalizedTarget = target % boardSize;
    if (normalizedTarget < 0) normalizedTarget += boardSize;

    // Sesuai rule tambahan: teleport yang melewati GO tetap dapat gaji.
    if (normalizedTarget < oldPos) {
        handlePassGo(player);
    }

    player->setPosition(normalizedTarget);

    if (board != nullptr) {
        Tile* tile = board->getTileAt(normalizedTarget);
        if (tile != nullptr) {
            tile->onLand(player);
        }
    }
}

void MovementHandler::pullPlayer(Player* target, int toPosition) {
    if (target == nullptr) {
        return;
    }

    if (target->getStatus() == BANKRUPT) {
        return;
    }

    // Pull memindahkan target lalu memicu efek tile tujuan (onLand).
    int normalizedTarget = toPosition % boardSize;
    if (normalizedTarget < 0) {
        normalizedTarget += boardSize;
    }

    target->setPosition(normalizedTarget);

    if (board != nullptr) {
        Tile* tile = board->getTileAt(normalizedTarget);
        if (tile != nullptr) {
            tile->onLand(target);
        }
    }
}

void MovementHandler::handlePassGo(Player* player) {
    if (!player) return;

    // Cari GoTile di board 
    if (board != nullptr) {
        Tile* goTile = board->getTileAt(0);
        if (goTile != nullptr) {
            goTile->onPass(player); 
            return;
        }
    }

    // Fallback 
    (*player) += goSalary;
}

void MovementHandler::sendToJail(Player* player) {
    if (!player) return;

    player->setStatus(JAILED);
    player->resetConsecutiveDoubles();
    player->resetJailTurns();

    JailTile* jailTile = findJailTile(board, boardSize);
    if (jailTile != nullptr) {
        jailTile->sendToJail(player);
        return;
    }

    // Fallback jika board belum punya JailTile
    player->setPosition(10);
}

JailResult MovementHandler::handleJailTurn(Player* player, Dice& dice) {
    if (!player) return STILL_JAILED;

    JailTile* jailTile = findJailTile(board, boardSize);
    int fine = (jailTile != nullptr) ? jailTile->getFineAmount() : jailFine;

    // Sudah 3 kali gagal → giliran ini wajib bayar denda
    if (player->getJailTurnsRemaining() >= 3) {
        if (jailTile != nullptr) {
            return jailTile->attemptEscape(player);
        } else {
            (*player) -= fine;
            player->setStatus(ACTIVE);
            player->resetJailTurns();
            return FORCED_OUT;
        }
    }

    // Coba roll
    if (dice.isDouble()) {
        player->setStatus(ACTIVE);
        player->resetJailTurns();
        return ESCAPED_DOUBLE;
    }

    

    player->incrementJailTurns();
    return STILL_JAILED;
}
