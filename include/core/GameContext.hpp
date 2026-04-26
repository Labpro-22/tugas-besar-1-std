#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include <vector>

// Forward declarations 
class GameBoard;
class TurnManager;
class MovementHandler;
class Player;
class SkillCardManager;
class Dice;
class GameView;
class BuildingManager;
class TransactionLogger;

// GameContext membawa referensi ke fungsi SkillCard, ActionCard, dan Tile saat execute().
//pointer di manage oleh GameController.

class GameContext {
public:
    GameBoard*         board            = nullptr;
    MovementHandler*   movementHandler  = nullptr;
    TurnManager*       turnManager      = nullptr;
    SkillCardManager*  skillCardManager = nullptr;
    BuildingManager*   buildingManager  = nullptr;
    Dice*              lastDice         = nullptr;  
    GameView*          gameView         = nullptr;  
    TransactionLogger* logger           = nullptr;  
    std::vector<Player*> allPlayers;                

    GameContext() = default;

    // Null check
    bool hasBoard()            const { 
        return board            
        != nullptr; 
    }

    bool hasMovementHandler()  const { 
        return movementHandler  
        != nullptr; 
    }
    bool hasTurnManager()      const { 
        return turnManager      
        != nullptr; 
    }
    bool hasSkillCardManager() const { 
        return skillCardManager 
        != nullptr; 
    }
    bool hasBuildingManager()  const { 
        return buildingManager  
        != nullptr; 
    }
    bool hasLastDice()         const { 
        return lastDice         
        != nullptr; 
    }
    bool hasLogger()           const { 
        return logger           
        != nullptr; 
    }

    // Siap jalanin turn
    bool isReadyForTurn() const {
        return hasBoard() && hasMovementHandler() && hasTurnManager();
    }

    // Ukuran board dgn fallback
    int getBoardSize() const;
};

#endif
