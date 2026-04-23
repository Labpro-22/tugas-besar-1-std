#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include <vector>
#include <string>
#include <memory>

class GameBoard;
class Bank;
class Auction;
class BuildingManager;
class Dice;
class TurnManager;
class MovementHandler;
class SkillCardManager;
class ConfigParser;
class GameSaver;
class GameLoader;
class TransactionLogger;
class WinConditionChecker;
class Player;
class Tile;
class Property;

class GameController {
private:
    GameBoard* gameBoard;
<<<<<<< HEAD
    ConfigParser* configParser;
    GameSaver* gameSaver;
    GameLoader* gameLoader;
    TransactionLogger* transactionLogger;
=======
    // Bank* bank;
    // Auction* auction;
    // BuildingManager* buildingManager;
    // Dice* dice;
    TurnManager* turnManager;
    // MovementHandler* movementHandler;
    // SkillCardManager* skillCardManager;
    // ConfigParser* configParser;
    // GameSaver* gameSaver;
    // GameLoader* gameLoader;
    // TransactionLogger* transactionLogger;
    // WinConditionChecker* winConditionChecker;
>>>>>>> a27281703938384cb8b2dbd641a38b059f28cfe8

public:
    GameController();
    ~GameController();

    void startGame();
    void initializeGame();
    void addPlayer(const std::string& username, int startingMoney);

    bool loadFromConfig(const std::string& basePath);
    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);
    TransactionLogger* getLogger() const;
    void logAction(const std::string& username, const std::string& action, const std::string& detail);

    void processTurn(Player& player, int diceResult);
    void processLanding(Player& player, Tile* tile);
    void processPurchase(Player& player, Property* property);
    void processAuction(Property* property);
    void processBankruptcy(Player& player);

    bool checkGameEnd();
    Player* getWinner();

    GameBoard& getBoard();
    const std::vector<std::shared_ptr<Player>>& getPlayers() const;
    std::shared_ptr<Player> getPlayerByUsername(const std::string& username);
};

#endif