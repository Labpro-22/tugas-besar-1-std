#include "../../include/core/GameController.hpp"
#include "../../include/models/GameBoard.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/models/Street.hpp"
#include "../../include/data/ConfigParser.hpp"
#include "../../include/data/GameSaver.hpp"
#include "../../include/data/GameLoader.hpp"
#include "../../include/data/TransactionLogger.hpp"
#include "../../include/utils/FileFormatException.hpp"
#include "../../include/utils/FileWriteException.hpp"
#include <iostream>

using namespace std;

GameController::GameController()
    : gameBoard(nullptr), configParser(nullptr),
      gameSaver(nullptr), gameLoader(nullptr),
      transactionLogger(nullptr) {
    gameBoard = new GameBoard();
    gameSaver = new GameSaver();
    gameLoader = new GameLoader();
    transactionLogger = new TransactionLogger();
}

GameController::~GameController() {
    delete gameBoard;
    delete configParser;
    delete gameSaver;
    delete gameLoader;
    delete transactionLogger;
}

void GameController::startGame() {
    cout << "[GameController] Game berhasil dimulai!\n";
    if (!loadFromConfig("../config")) {
        cout << "[GameController] WARNING: gagal memuat config/, board kosong.\n";
    }
}

bool GameController::loadFromConfig(const std::string& basePath) {
    try {
        delete configParser;
        configParser = new ConfigParser(basePath);
        configParser->loadConfig(gameBoard);
        if (configParser->getMaxTurn() > 0) {
            gameBoard->setMaxTurn(configParser->getMaxTurn());
        }
        cout << "[GameController] Config dimuat dari " << basePath
             << " (" << gameBoard->getTiles().size() << " petak, MAX_TURN="
             << gameBoard->getMaxTurn() << ")\n";
        return true;
    } catch (const FileFormatException& e) {
        cout << "[GameController] ConfigParser error: " << e.what() << endl;
        return false;
    }
}

bool GameController::saveGame(const std::string& filename) {
    try {
        gameSaver->save(gameBoard, transactionLogger, filename);
        cout << "[GameController] Game disimpan ke " << filename << endl;
        return true;
    } catch (const FileWriteException& e) {
        cout << "[GameController] GameSaver error: " << e.what() << endl;
        return false;
    }
}

bool GameController::loadGame(const std::string& filename) {
    try {
        if (!gameLoader->validate(filename)) {
            cout << "[GameController] Save file invalid: " << filename << endl;
            return false;
        }
        gameLoader->loadSave(filename, gameBoard, transactionLogger);
        cout << "[GameController] Game dimuat dari " << filename << endl;
        return true;
    } catch (const FileFormatException& e) {
        cout << "[GameController] GameLoader error: " << e.what() << endl;
        return false;
    }
}

TransactionLogger* GameController::getLogger() const {
    return transactionLogger;
}

void GameController::logAction(const std::string& username,
                               const std::string& action,
                               const std::string& detail) {
    if (transactionLogger == nullptr) return;
    int turn = (gameBoard != nullptr) ? gameBoard->getCurrentTurnNumber() : 0;
    transactionLogger->log(turn, username, action, detail);
}

void GameController::addPlayer(const std::string& username, int startingMoney) {
    if (gameBoard != nullptr) {
        std::shared_ptr<Player> newPlayer = std::make_shared<Player>(username, startingMoney);
        gameBoard->addPlayer(newPlayer);
        cout << "[GameController] Player " << username << " ditambahkan dengan uang " << startingMoney << endl;
        logAction(username, "JOIN", "saldo awal " + std::to_string(startingMoney));
    }
}

void GameController::processTurn(Player& player, int diceResult) {
    (void)player;
    (void)diceResult;

    cout << "[GameController] processTurn\n";

    // nanti:
    // movementHandler->movePlayer(...)
}

void GameController::processLanding(Player& player, Tile* tile) {
    (void)player;

    if (tile == nullptr) return;

    cout << "[GameController] processLanding di " << tile->getName() << endl;

    // nanti:
    // tile->onLand(player)
}

void GameController::processPurchase(Player& player, Property* property) {
    (void)player;

    if (property == nullptr) return;

    cout << "[GameController] processPurchase " << property->getName() << endl;

    // nanti:
    // bank->sellProperty(...)
}

void GameController::processAuction(Property* property) {
    if (property == nullptr) return;

    cout << "[GameController] processAuction\n";

    // nanti:
    // auction->startAuction()
}

void GameController::processBankruptcy(Player& player) {
    cout << "[GameController] processBankruptcy: " << player.getUsername() << endl;

    // nanti:
    // bank->declareBankruptcy(...)
}

bool GameController::checkGameEnd() {
    cout << "[GameController] checkGameEnd\n";

    // nanti:
    // return winConditionChecker->isGameOver(...)
    return false;
}

Player* GameController::getWinner() {
    cout << "[GameController] getWinner\n";
    return nullptr; // TODO: implement win condition
}

// ==========================
// GETTER UNTUK UI (WAJIB)
// ==========================

GameBoard& GameController::getBoard() {
    return *gameBoard;
}

const std::vector<std::shared_ptr<Player>>& GameController::getPlayers() const {
    return gameBoard->getPlayers();
}

std::shared_ptr<Player> GameController::getPlayerByUsername(const std::string& username) {
    return gameBoard->getPlayerByUsername(username);
}