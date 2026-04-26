#ifndef COMMAND_PROCESSOR_HPP
#define COMMAND_PROCESSOR_HPP

#include <string>
#include <random>
#include "../include/core/GameController.hpp"
#include "../include/views/BoardView.hpp"
#include "../include/views/PropertyView.hpp"
#include "../include/views/GameView.hpp"
#include "../../include/data/TransactionLogger.hpp"
#include "../../include/core/TurnManager.hpp"
#include "../../include/utils/Dice.hpp"


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

public:
    CommandProcessor(GameController* gc, BoardView* bv, PropertyView* pv, GameView* gv);
    void readCommand();
    void executeCommand(const std::string& cmd);
};

#endif