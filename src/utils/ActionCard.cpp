#include "../../include/utils/ActionCard.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/core/GameContext.hpp"
#include "../../include/core/SkillCardManager.hpp"
#include "../../include/core/MovementHandler.hpp"

ChanceCard::ChanceCard(ActionCardType t, int v, string d)
    : type(t), value(v), desc(d) {}

CommunityCard::CommunityCard(ActionCardType t, int v, string d)
    : type(t), value(v), desc(d) {}

void ChanceCard::execute(Player* player, GameContext* ctx) {
    if (player == nullptr || ctx == nullptr) {
        return;
    }

    switch (type) {
        case ActionCardType::GAIN_MONEY:
            (*player) += value;
            break;
        case ActionCardType::PAY_MONEY:
            (*player) -= value;
            break;
        case ActionCardType::MOVE:
            if (ctx->movementHandler != nullptr) {
                ctx->movementHandler->movePlayer(player, value);
            }
            break;
        case ActionCardType::TELEPORT:
            if (ctx->movementHandler != nullptr) {
                ctx->movementHandler->teleportPlayer(player, value);
            }
            break;
        case ActionCardType::GO_TO_JAIL:
            if (ctx->movementHandler != nullptr) {
                ctx->movementHandler->sendToJail(player);
            }
            break;
        case ActionCardType::GET_SKILL:
            if (ctx->skillCardManager != nullptr) {
                ctx->skillCardManager->distributeCardTo(player);
            }
            break;
    }
}

string ChanceCard::getDescription() { return desc; }

void CommunityCard::execute(Player* player, GameContext* ctx) {
    if (!player || !ctx) return;

    switch (type) {
        case ActionCardType::COLLECT_FROM_ALL:
            for (Player* other : ctx->allPlayers) {
                if (other != player && other->getStatus() == ACTIVE) {
                    (*other) -= value;
                    (*player) += value;
                }
            }
            break;
        case ActionCardType::PAY_TO_ALL:
            for (Player* other : ctx->allPlayers) {
                if (other != player && other->getStatus() == ACTIVE) {
                    (*player) -= value;
                    (*other) += value;
                }
            }
            break;
        case ActionCardType::PAY_MONEY:
            (*player) -= value;
            break;
        default:
            break;
    }
}

string CommunityCard::getDescription() { return desc; }
