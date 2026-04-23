#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H
#include "../models/GameBoard.hpp"
#include <vector>
using namespace std;

class GameBoard;
class TurnManager;
class MovementHandler;
class Player;
class SkillCardManager;
class Dice;

class GameContext {
public:
    GameBoard* board;
    MovementHandler* movementHandler;
    TurnManager* turnManager;
    SkillCardManager* skillCardManager;
    Dice* lastDice;
    std::vector<Player*> allPlayers;

    GameContext()
        : board(nullptr), movementHandler(nullptr), turnManager(nullptr),
          skillCardManager(nullptr), lastDice(nullptr), allPlayers() {}

    bool hasBoard() const { return board != nullptr; }
    bool hasMovementHandler() const { return movementHandler != nullptr; }
    bool hasTurnManager() const { return turnManager != nullptr; }
    bool hasSkillCardManager() const { return skillCardManager != nullptr; }
    bool hasLastDice() const { return lastDice != nullptr; }

    bool isReadyForTurn() const {
        return hasBoard() && hasMovementHandler() && hasTurnManager();
    }

    // Helper supaya kartu tidak perlu akses board langsung
    int getBoardSize() const {
        if (board != nullptr && !board->getTiles().empty()) return board->getTiles().size();
        return 40;
    }
};

#endif
