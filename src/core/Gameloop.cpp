#include "../../include/core/GameLoop.hpp"
#include "../../include/core/GameContext.hpp"
#include "../../include/core/TurnManager.hpp"
#include "../../include/core/MovementHandler.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/utils/Dice.hpp"

void GameLoop::playTurn(GameContext* ctx, Dice& dice) {
    if (ctx == nullptr || !ctx->isReadyForTurn()) {
        return;
    }

    Player* p = ctx->turnManager->getCurrentPlayer();
    if (p == nullptr) {
        return;
    }

    ctx->turnManager->resetTurnFlags(p);

    // Jail
    if (p->getStatus() == JAILED) {
        JailResult res = ctx->movementHandler->handleJailTurn(p, dice);

        if (res == STILL_JAILED) {
            ctx->turnManager->advanceToNextPlayer();
            return;
        }
    }

    // Roll dice
    dice.rollRandom();
    p->markRolled();

    if (dice.isDouble()) {
        p->incrementConsecutiveDoubles();
    } else {
        p->resetConsecutiveDoubles();
    }

    // triple double
    if (p->getConsecutiveDoubles() == 3) {
        ctx->movementHandler->sendToJail(p);
        p->resetConsecutiveDoubles();
        ctx->turnManager->advanceToNextPlayer();
        return;
    }

    // Move player
    ctx->movementHandler->movePlayer(p, dice.getTotal());

    // TODO: tile effect lanjut di tim lain

    // end turn
    if (!dice.isDouble()) {
        ctx->turnManager->advanceToNextPlayer();
    }
}
