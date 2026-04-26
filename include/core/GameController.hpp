#ifndef GAME_CONTROLLER_HPP
#define GAME_CONTROLLER_HPP

#include <vector>
#include <string>
#include <memory>
#include "GameContext.hpp"
#include "../models/WinConditionChecker.hpp"

class GameBoard;
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
class GameView;

class GameController {
private:
    GameBoard* gameBoard;
    ConfigParser* configParser;
    GameSaver* gameSaver;
    GameLoader* gameLoader;
    TransactionLogger* transactionLogger;
    Auction* auction;
    BuildingManager* buildingManager;
    Dice* dice;
    TurnManager* turnManager;
    MovementHandler* movementHandler;
    SkillCardManager* skillCardManager;
    WinConditionChecker* winConditionChecker;
    GameContext gameContext;
    int currentTurn;

public:
    GameController();
    ~GameController();

    std::string startGame();
    void initializeGame();
    std::string addPlayer(const std::string& username, int startingMoney);

    bool loadFromConfig(const std::string& basePath);
    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);
    TransactionLogger* getLogger() const;
    void logAction(const std::string& username, const std::string& action, const std::string& detail);

    void processTurn(Player& player, int diceResult);
    std::string processLanding(Player& player, Tile* tile);
    void processPurchase(Player& player, Property* property);
    void processAuction(Property* property);
    void processBankruptcy(Player& player, Player* creditor = nullptr);

    bool isAuctionActive() const;
    bool isAuctionEnded() const;
    bool placeAuctionBid(const std::string& username, int amount);
    void passAuction(const std::string& username);
    bool finalizeAuction();
    int getAuctionCurrentBid() const;
    std::string getAuctionCurrentBidderName() const;

    bool mortgage(Player& player, Property* property);
    bool redeem(Player& player, Property* property);

    bool checkGameEnd();
    Player* getWinner();
    std::vector<Player*> getWinners();

    GameBoard& getBoard();
    const std::vector<std::shared_ptr<Player>>& getPlayers() const;
    std::shared_ptr<Player> getPlayerByUsername(const std::string& username);

    MovementHandler*  getMovementHandler()  const;
    TurnManager*      getTurnManager()      const;
    SkillCardManager* getSkillCardManager() const;
};

#endif