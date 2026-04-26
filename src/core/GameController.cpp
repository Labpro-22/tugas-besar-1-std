#include "../../include/core/GameController.hpp"
#include "../../include/core/SkillCardManager.hpp"
#include "../../include/models/GameBoard.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/models/Property.hpp"
#include "../../include/models/Street.hpp"
#include "../../include/models/Tile.hpp"
#include "../../include/models/Auction.hpp"
#include "../../include/models/BuildingManager.hpp"
#include "../../include/models/WinConditionChecker.hpp"
#include "../../include/data/ConfigParser.hpp"
#include "../../include/data/GameSaver.hpp"
#include "../../include/data/GameLoader.hpp"
#include "../../include/data/TransactionLogger.hpp"
#include "../../include/utils/FileFormatException.hpp"
#include "../../include/utils/FileWriteException.hpp"
#include "../../include/core/MovementHandler.hpp"
#include "../../include/core/TurnManager.hpp"
#include "../../include/core/GameContext.hpp"
#include "../../include/utils/Dice.hpp"

#include <iostream>
#include <algorithm>

using namespace std;


// Constructor / Destructor

GameController::GameController()
    : gameBoard(nullptr), configParser(nullptr),
      gameSaver(nullptr), gameLoader(nullptr),
      transactionLogger(nullptr),
      auction(nullptr),
      buildingManager(nullptr), dice(nullptr),
      turnManager(nullptr), movementHandler(nullptr),
      skillCardManager(nullptr), winConditionChecker(nullptr),
      currentTurn(0) {
    gameBoard         = new GameBoard();
    gameSaver         = new GameSaver();
    gameLoader        = new GameLoader();
    transactionLogger = new TransactionLogger();
    buildingManager   = new BuildingManager();
    skillCardManager  = new SkillCardManager();
}

GameController::~GameController() {
    delete gameBoard;
    delete configParser;
    delete gameSaver;
    delete gameLoader;
    delete transactionLogger;
    delete buildingManager;
    delete dice;
    delete turnManager;
    delete movementHandler;
    delete skillCardManager;
    delete winConditionChecker;
    delete auction;
}

// Initialization

void GameController::initializeGame() {
    gameContext = GameContext();
    gameContext.board            = gameBoard;
    gameContext.buildingManager  = buildingManager;
    gameContext.skillCardManager = skillCardManager;
    gameContext.logger           = transactionLogger;

    delete dice;
    dice = new Dice();
    gameContext.lastDice = dice;

    vector<Player*> rawPlayers;
    for (auto& sp : gameBoard->getPlayers()) {
        rawPlayers.push_back(sp.get());
    }
    delete turnManager;
    turnManager = new TurnManager(rawPlayers, gameBoard->getMaxTurn());
    gameContext.turnManager = turnManager;

    delete movementHandler;
    movementHandler = new MovementHandler(gameBoard, &gameContext);
    gameContext.movementHandler = movementHandler;

    skillCardManager->initDeck();
    gameBoard->setSkillDeck(&skillCardManager->getDeck());

    delete winConditionChecker;
    winConditionChecker = new WinConditionChecker(turnManager, gameBoard);

    turnManager->syncWithBoard(gameBoard);

    currentTurn = 0;

    logAction("SYSTEM", "INIT", "Game diinisialisasi dengan " +
              to_string(rawPlayers.size()) + " pemain");
}

string GameController::startGame() {
    string msg = "Game berhasil dimulai!";

    if (!loadFromConfig("config")) {
        msg += "\nWARNING: gagal memuat config/, board kosong.";
    }

    return msg;
}

// Config / Save / Load

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

// Logger / Players

TransactionLogger* GameController::getLogger() const {
    return transactionLogger;
}

void GameController::logAction(const std::string& username, const std::string& action, const std::string& detail) {
    if (transactionLogger == nullptr) return;
    currentTurn = (gameBoard != nullptr) ? gameBoard->getCurrentTurnNumber() : 0;
    transactionLogger->log(currentTurn, username, action, detail);
}

string GameController::addPlayer(const string& username, int money) {
    if (!gameBoard) return "Board tidak tersedia";

    auto p = make_shared<Player>(username, money);
    gameBoard->addPlayer(p);

    logAction(username, "JOIN", "saldo awal " + to_string(money));

    return "Player " + username + " ditambahkan dengan uang " + to_string(money);
}

// processTurn

void GameController::processTurn(Player& player, int diceResult) {
    if (!movementHandler) return;
    movementHandler->movePlayer(&player, diceResult);
    logAction(player.getUsername(), "LEMPAR_DADU", "hasil " + to_string(diceResult));
}

// processLanding

string GameController::processLanding(Player& player, Tile* tile) {
    if (!tile) return "";

    Property* prop = dynamic_cast<Property*>(tile);
    if (!prop) return "Mendarat di " + tile->getName();

    if (prop->getStatus() == StatusType::BANK) {
        return "Mendarat di " + tile->getName() + " (belum dimiliki)";
    }

    if (prop->getStatus() == StatusType::MORTGAGED) {
        return "Mendarat di " + tile->getName() + " (digadai, tidak ada sewa)";
    }

    if (prop->getOwner() == player.getUsername()) {
        return "Mendarat di properti sendiri: " + tile->getName();
    }

    int rent = prop->calculateRent(&player);
    auto ownerPtr = gameBoard->getPlayerByUsername(prop->getOwner());
    if (!ownerPtr) return "Mendarat di " + tile->getName();

    player    -= rent;
    *ownerPtr += rent;

    logAction(player.getUsername(), "BAYAR_SEWA",
              prop->getName() + " milik " + ownerPtr->getUsername() +
              " sebesar M" + to_string(rent));

    if (player.getMoney() < 0) {
        processBankruptcy(player, ownerPtr.get());
    }

    return "Mendarat di " + tile->getName();
}

// processPurchase

void GameController::processPurchase(Player& player, Property* property) {
    if (!property) return;
    if (property->getStatus() != StatusType::BANK) return;

    int price = property->getPurchasePrice();

    int discount = player.getDiscountPercent();
    if (discount > 0) {
        price = price * (100 - discount) / 100;
        player.clearDiscount();
    }

    if (player.getMoney() < price) return;

    player.payVoluntary(price);
    property->setOwner(player.getUsername());
    property->setStatus(StatusType::OWNED);

    logAction(player.getUsername(), "BELI_PROPERTI",
              property->getName() + " seharga M" + to_string(price));
}

// processAuction

void GameController::processAuction(Property* property) {
    if (!property) return;
    if (property->getStatus() != StatusType::BANK) return;

    vector<shared_ptr<Player>> activePlayers;
    for (auto& p : gameBoard->getPlayers()) {
        if (p->getStatus() == ACTIVE) activePlayers.push_back(p);
    }
    if (activePlayers.empty()) return;

    auto sharedProp = shared_ptr<Property>(property, [](Property*){});

    delete auction;
    auction = new Auction(sharedProp, activePlayers);
    auction->startAuction();
    // Interaksi bid/pass digerakkan oleh CommandProcessor via auction->placeBid() / auction->pass()
}

bool GameController::isAuctionActive() const {
    return auction != nullptr && auction->isActive();
}

bool GameController::isAuctionEnded() const {
    return auction != nullptr && auction->isAuctionEnded();
}

bool GameController::placeAuctionBid(const std::string& username, int amount) {
    if (!auction || !auction->isActive()) return false;
    auto bidder = gameBoard->getPlayerByUsername(username);
    if (!bidder || bidder->getStatus() == BANKRUPT) return false;
    bool ok = auction->placeBid(bidder, amount);
    if (ok) {
        logAction(username, "BID", "menawar M" + to_string(amount));
    }
    return ok;
}

void GameController::passAuction(const std::string& username) {
    if (!auction || !auction->isActive()) return;
    auto bidder = gameBoard->getPlayerByUsername(username);
    if (!bidder || bidder->getStatus() == BANKRUPT) return;
    auction->pass(bidder);
    logAction(username, "PASS", "melewati giliran lelang");
}

bool GameController::finalizeAuction() {
    if (!auction || !auction->isActive()) return false;

    auto winner = auction->getWinner();
    auto sharedProp = auction->getProperty();
    Property* prop = sharedProp.get();
    int finalBid = auction->getCurrentBid();

    if (winner && prop && prop->getStatus() == StatusType::BANK && winner->getMoney() >= finalBid) {
        winner->payVoluntary(finalBid);
        prop->setOwner(winner->getUsername());
        prop->setStatus(StatusType::OWNED);
        logAction(winner->getUsername(), "MENANG_LELANG",
                  prop->getName() + " seharga M" + to_string(finalBid));
    }

    auction->endAuction();
    delete auction;
    auction = nullptr;
    return true;
}

int GameController::getAuctionCurrentBid() const {
    if (!auction) return 0;
    return auction->getCurrentBid();
}

std::string GameController::getAuctionCurrentBidderName() const {
    if (!auction) return "";
    auto bidder = auction->getCurrentBidder();
    return bidder ? bidder->getUsername() : "";
}

// processBankruptcy

void GameController::processBankruptcy(Player& player, Player* creditor) {
    if (player.getStatus() == BANKRUPT) return;

    cout << "=== Panel Likuidasi ===\n";
    cout << "Debitur   : " << player.getUsername() << "\n";
    if (creditor) {
        cout << "Kreditor  : " << creditor->getUsername() << "\n";
    } else {
        cout << "Kreditor  : BANK\n";
    }
    cout << "Kas akhir : M" << player.getMoney() << "\n";

    player.setStatus(BANKRUPT);

    vector<string> transferredAssets;

    for (const auto& tilePtr : gameBoard->getTiles()) {
        Property* prop = dynamic_cast<Property*>(tilePtr.get());
        if (!prop) continue;
        if (prop->getOwner() != player.getUsername()) continue;

        if (creditor && creditor->getStatus() != BANKRUPT) {
            prop->setOwner(creditor->getUsername());
            if (prop->getStatus() == StatusType::BANK) {
                prop->setStatus(StatusType::OWNED);
            }
            transferredAssets.push_back(prop->getName());
        } else {
            prop->setOwner("");
            prop->setStatus(StatusType::BANK);
        }

        prop->setFestivalMultiplier(1);
        prop->setFestivalDuration(0);

        Street* street = dynamic_cast<Street*>(prop);
        if (street) street->setBuildingCount("0");
    }

    if (creditor && creditor->getStatus() != BANKRUPT) {
        int remainingMoney = player.getMoney();
        if (remainingMoney > 0) {
            *creditor += remainingMoney;
            player.payVoluntary(remainingMoney);
        }
        logAction(player.getUsername(), "BANGKRUT",
                  "aset dialihkan ke " + creditor->getUsername());
        cout << "Aset dialihkan ke " << creditor->getUsername() << ":\n";
        for (const string& asset : transferredAssets) {
            cout << "  - " << asset << "\n";
        }
    } else {
        logAction(player.getUsername(), "BANGKRUT",
                  "dikeluarkan dari permainan, semua properti dikembalikan ke bank");
        cout << "Seluruh aset dikembalikan ke BANK.\n";
    }

    cout << "[GameController] " << player.getUsername() << " bangkrut.\n";
}

// mortgage / redeem

bool GameController::mortgage(Player& player, Property* property) {
    if (!property) return false;
    if (property->getOwner() != player.getUsername()) return false;
    if (property->getStatus() != StatusType::OWNED) return false;

    int value = property->getMortgageValue();
    property->mortgage();
    player += value;

    logAction(player.getUsername(), "GADAI",
              property->getName() + " nilai M" + to_string(value));
    return true;
}

bool GameController::redeem(Player& player, Property* property) {
    if (!property) return false;
    if (property->getOwner() != player.getUsername()) return false;
    if (property->getStatus() != StatusType::MORTGAGED) return false;

    int cost = property->getPurchasePrice();
    if (player.getMoney() < cost) return false;

    player.payVoluntary(cost);
    property->redeem();

    logAction(player.getUsername(), "TEBUS",
              property->getName() + " biaya M" + to_string(cost));
    return true;
}

// checkGameEnd / getWinner

bool GameController::checkGameEnd() {
    if (winConditionChecker)
        return winConditionChecker->isGameOver(gameBoard->getPlayers());

    int activeCnt = 0;
    for (auto& p : gameBoard->getPlayers()) {
        if (p->getStatus() != BANKRUPT) activeCnt++;
    }
    return activeCnt <= 1;
}

Player* GameController::getWinner() {
    if (winConditionChecker)
        return winConditionChecker->determineWinner(gameBoard->getPlayers());

    Player* winner  = nullptr;
    int     maxMoney = -1;
    for (auto& p : gameBoard->getPlayers()) {
        if (p->getStatus() == BANKRUPT) continue;
        if (p->getMoney() > maxMoney) {
            maxMoney = p->getMoney();
            winner   = p.get();
        }
    }
    return winner;
}

std::vector<Player*> GameController::getWinners() {
    if (winConditionChecker)
        return winConditionChecker->determineWinners(gameBoard->getPlayers());

    Player* winner = getWinner();
    if (!winner) return {};
    return { winner };
}

// Getters

GameBoard& GameController::getBoard() {
    return *gameBoard;
}

const std::vector<std::shared_ptr<Player>>& GameController::getPlayers() const {
    return gameBoard->getPlayers();
}

std::shared_ptr<Player> GameController::getPlayerByUsername(const std::string& username) {
    return gameBoard->getPlayerByUsername(username);
}

MovementHandler* GameController::getMovementHandler() const {
    return movementHandler;
}

TurnManager* GameController::getTurnManager() const {
    return turnManager;
}

SkillCardManager* GameController::getSkillCardManager() const {
    return skillCardManager;
}
