#ifndef COMMAND_PROCESSOR_HPP
#define COMMAND_PROCESSOR_HPP

#include <string>
#include <random>
#include <vector>
#include "../include/core/GameController.hpp"
#include "../include/views/BoardView.hpp"
#include "../include/views/PropertyView.hpp"
#include "../include/views/GameView.hpp"
#include "../../include/data/TransactionLogger.hpp"
#include "../../include/core/TurnManager.hpp"
#include "../../include/utils/Dice.hpp"
#include "../../include/models/Property.hpp"


class GameController;
class BoardView;
class PropertyView;
class GameView;

class CommandProcessor {
private:
    GameController* gameController;
    BoardView* boardView;
    PropertyView* propertyView;
    GameView* gameView;
    Dice dice;

    Property* pendingProperty;
    std::string pendingPlayer;
    bool pendingNeedsDecision;
    bool pendingExtraTurn;

    std::vector<std::shared_ptr<Player>> auctionOrder;
    int auctionIndex;
    bool auctionRunning;

public:
    CommandProcessor(GameController* gc, BoardView* bv, PropertyView* pv, GameView* gv);
    void readCommand();
    void executeCommand(const std::string& cmd);
};

#endif