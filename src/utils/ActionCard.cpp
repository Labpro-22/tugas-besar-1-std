#include "../../include/utils/ActionCard.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/core/GameContext.hpp"
#include "../../include/models/GameBoard.hpp"
#include "../../include/core/SkillCardManager.hpp"
#include "../../include/core/MovementHandler.hpp"
#include "../../include/models/Railroad.hpp"


ChanceCard::ChanceCard(ActionCardType t, int v, string d)
    : type(t), value(v), desc(d) {}

CommunityCard::CommunityCard(ActionCardType t, int v, string d)
    : type(t), value(v), desc(d) {}


// Helper cari posisi stasiun (Railroad) terdekat di depan player

namespace {

int findNearestRailroadPosition(Player* player, GameContext* ctx) {
    if (!player || !ctx || !ctx->hasBoard()) return -1;

    const auto& tiles = ctx->board->getTiles();
    int boardSize = static_cast<int>(tiles.size());
    int myPos = player->getPosition();

    // Cari Railroad dengan jarak clockwise terkecil
    int nearestPos  = -1;
    int minDist     = INT_MAX;

    for (const auto& tilePtr : tiles) {
        if (!tilePtr) continue;
        Railroad* rr = dynamic_cast<Railroad*>(tilePtr.get());
        if (!rr) continue;

        int rrPos = rr->getPosition();
        int dist  = (rrPos - myPos + boardSize) % boardSize;
        if (dist > 0 && dist < minDist) {
            minDist    = dist;
            nearestPos = rrPos;
        }
    }

    return nearestPos; // -1 jika tidak ada Railroad
}

} 



void ChanceCard::execute(Player* player, GameContext* ctx) {
    if (!player || !ctx) return;

    switch (type) {
        case ActionCardType::NEAREST_RAILROAD: {
            // Teleport ke stasiun terdekat 
            if (!ctx->hasMovementHandler()) break;
            int target = findNearestRailroadPosition(player, ctx);
            if (target >= 0) {
                ctx->movementHandler->teleportPlayer(player, target);
            }
            break;
        }

        case ActionCardType::MOVE_BACKWARD: {
            if (!ctx->hasMovementHandler()) break;
            int boardSize = ctx->getBoardSize();
            int myPos     = player->getPosition();
            int newPos    = (myPos - value + boardSize) % boardSize;
            player->setPosition(newPos);
            if (ctx->hasBoard()) {
                auto* tile = ctx->board->getTileAt(newPos);
                if (tile) tile->onLand(player, ctx);
            }
            break;
        }

        case ActionCardType::GO_TO_JAIL: {
            if (!ctx->hasMovementHandler()) break;
            ctx->movementHandler->sendToJail(player);
            break;
        }

        case ActionCardType::GAIN_MONEY:
            (*player) += value;
            break;

        case ActionCardType::PAY_MONEY:
            (*player) -= value;
            break;

        default:
            break;
    }
}

string ChanceCard::getDescription() { return desc; }

void CommunityCard::execute(Player* player, GameContext* ctx) {
    if (!player || !ctx) return;

    switch (type) {
        case ActionCardType::COLLECT_FROM_ALL: {
            for (Player* other : ctx->allPlayers) {
                if (!other)                            continue;
                if (other == player)                   continue;
                if (other->getStatus() != ACTIVE)     continue;

                (*other)  -= value;
                (*player) += value;
            }
            break;
        }

        case ActionCardType::PAY_TO_ALL: {
            // Shield berlaku untuk total pembayaran (satu kali blok).
            for (Player* other : ctx->allPlayers) {
                if (!other)                            continue;
                if (other == player)                   continue;
                if (other->getStatus() != ACTIVE)     continue;

                (*player) -= value; // operator-= cek shield per panggilan
                (*other)  += value;
            }
            break;
        }

        case ActionCardType::PAY_MONEY:
            (*player) -= value;
            break;

        case ActionCardType::GAIN_MONEY:
            (*player) += value;
            break;

        default:
            break;
    }
}

string CommunityCard::getDescription() { return desc; }
