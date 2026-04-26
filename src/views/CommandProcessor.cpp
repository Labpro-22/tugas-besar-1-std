#include "../../include/views/CommandProcessor.hpp"
#include "../../include/core/MovementHandler.hpp"
#include "../../include/core/TurnManager.hpp"
#include "../../include/core/SkillCardManager.hpp"
#include "../../include/models/GameBoard.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/models/Tile.hpp"
#include "../../include/models/Property.hpp"
#include "../../include/models/Street.hpp"
#include "../../include/models/BuildingManager.hpp"
#include "../../include/utils/SkillCard.hpp"
#include "../../include/core/GameContext.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <climits>
#include <algorithm>

using namespace std;

CommandProcessor::CommandProcessor(GameController* gc, BoardView* bv, PropertyView* pv, GameView* gv)
        : gameController(gc), boardView(bv), propertyView(pv), gameView(gv),
            pendingProperty(nullptr), pendingNeedsDecision(false), pendingExtraTurn(false),
            auctionIndex(0), auctionRunning(false) {}

void CommandProcessor::readCommand() {
    string input;
    cout << "> ";
    getline(cin, input);

    executeCommand(input);
}

// Shared post-move logic: distribute skill card, handle overflow, log turn.
// Returns true if the player gets an extra turn (rolled double and didn't go to jail).
static bool handlePostMove(Player* player, GameView* gv, SkillCardManager* scm, bool rolledDouble) {
    if (scm) {
        SkillCard* newCard = scm->distributeCardTo(player);
        if (newCard && gv) {
            gv->showNewSkillCard(newCard->getDescription());
        }
        if (scm->isHandOverflow(player) && gv) {
            gv->showSkillOverflowWarning();
            vector<string> cardNames;
            for (SkillCard* c : player->getHand()) {
                cardNames.push_back(c->getDescription());
            }
            gv->showSkillCardsWithNew(cardNames);
            int dropIdx = gv->askDiscardSkill(static_cast<int>(player->getHand().size()));
            if (dropIdx >= 1 && dropIdx <= static_cast<int>(player->getHand().size())) {
                scm->dropCard(player, dropIdx - 1);
            }
        }
    }
    return rolledDouble;
}

static void showAutoWinIfNeeded(GameController* gc, GameView* gv) {
    if (!gc || !gv) return;
    if (!gc->checkGameEnd()) return;

    vector<Player*> winners = gc->getWinners();
    if (winners.empty()) return;

    if (winners.size() == 1) {
        gv->showMessage("MENANG! Pemenang: " + winners.front()->getUsername());
        return;
    }

    string joined;
    for (size_t i = 0; i < winners.size(); ++i) {
        if (i > 0) joined += ", ";
        joined += winners[i]->getUsername();
    }
    gv->showMessage("MENANG BERSAMA: " + joined);
}

static void advanceTurnAndCheck(GameController* gc, GameView* gv) {
    if (!gc) return;
    TurnManager* tm = gc->getTurnManager();
    if (!tm) return;

    tm->advanceToNextPlayer();
    tm->syncWithBoard(&gc->getBoard());

    Player* next = tm->getCurrentPlayer();
    tm->resetTurnFlags(next);

    if (gv && next) {
        int turn = tm->getCurrentTurnNumber();
        int maxT = tm->getMaxTurn();
        cout << "\n========================================\n";
        cout << "  GILIRAN: " << next->getUsername()
             << " | Uang: M" << next->getMoney()
             << " | Turn: " << turn
             << (maxT > 0 ? "/" + to_string(maxT) : "")
             << "\n";

        // Tampilkan kartu di tangan
        const auto& hand = next->getHand();
        if (hand.empty()) {
            cout << "  Kartu: (tidak ada)\n";
        } else {
            cout << "  Kartu kemampuan (" << hand.size() << "):\n";
            for (int i = 0; i < (int)hand.size(); ++i) {
                cout << "    " << (i+1) << ". " << hand[i]->getDescription() << "\n";
            }
        }
        cout << "========================================\n\n";
    }

    if (gv) showAutoWinIfNeeded(gc, gv);
}

static void finalizeLandingResolution(GameController* gc, GameView* gv, Player* player, bool extraTurn) {
    if (!gc || !gv || !player) return;

    if (player->getStatus() == BANKRUPT) {
        advanceTurnAndCheck(gc, gv);
        return;
    }

    if (extraTurn && player->getStatus() != JAILED) {
        gv->showMessage(player->getUsername() + " mendapat giliran tambahan karena double!");
        player->resetTurnFlags();
        return;
    }

    advanceTurnAndCheck(gc, gv);
}

// Core movement execution shared by LEMPAR_DADU and ATUR_DADU.
static void executeDiceMove(Player* player, Dice& dice, GameController* gc, GameView* gv,
                            Property*& pendingProperty, string& pendingPlayer,
                            bool& pendingNeedsDecision, bool& pendingExtraTurn) {
    if (!player || !gc || !gv) return;

    if (player->hasRolled()) {
        gv->showError("Kamu sudah melempar dadu di giliran ini.");
        return;
    }

    int total = dice.getTotal();
    bool isDouble = dice.isDouble();

    MovementHandler*  mh  = gc->getMovementHandler();
    TurnManager*      tm  = gc->getTurnManager();
    SkillCardManager* scm = gc->getSkillCardManager();

    bool wentToJail    = false;
    bool extraTurn     = false;

    if (player->getStatus() == JAILED) {
        if (!mh) {
            gv->showError("MovementHandler tidak tersedia.");
            return;
        }
        JailResult result = mh->handleJailTurn(player, dice);
        player->markRolled();

        if (result == JailResult::STILL_JAILED) {
            gv->showMessage(player->getUsername() + " masih di penjara.");
            advanceTurnAndCheck(gc, gv);
            return;
        }
        // ESCAPED_DOUBLE or FORCED_OUT: player is now active, move normally
        gv->showPlayerMove(player->getUsername(), total);
        if (mh) mh->movePlayer(player, total);

        if (player->getMoney() < 0) {
            gv->showMessage(player->getUsername() + " bangkrut setelah membayar denda penjara!");
            gc->processBankruptcy(*player);
            advanceTurnAndCheck(gc, gv);
            return;
        }

        handlePostMove(player, gv, scm, false);
        finalizeLandingResolution(gc, gv, player, false);
        return;
    }

    // Normal turn (not jailed)
    if (isDouble) {
        player->incrementConsecutiveDoubles();
        if (player->getConsecutiveDoubles() >= 3) {
            gv->showMessage(player->getUsername() + " melempar double 3 kali berturut-turut — masuk penjara!");
            player->markRolled();
            if (mh) mh->sendToJail(player);
            advanceTurnAndCheck(gc, gv);
            return;
        }
        extraTurn = true;
    } else {
        player->resetConsecutiveDoubles();
    }

    gv->showPlayerMove(player->getUsername(), total);
    if (mh) {
        mh->movePlayer(player, total);
    } else {
        // fallback: direct position set (no passGo)
        int sz = static_cast<int>(gc->getBoard().getTiles().size());
        player->setPosition(sz > 0 ? (player->getPosition() + total) % sz : 0);
    }

    player->markRolled();

    // Check if movePlayer sent us to jail (GoToJailTile)
    if (player->getStatus() == JAILED) {
        wentToJail = true;
        extraTurn  = false;
    }

    Tile* tile = gc->getBoard().getTileAt(player->getPosition());
    gv->showLanding(tile ? tile->getName() : "Unknown");

    if (!wentToJail && tile) {
        Property* landedProperty = dynamic_cast<Property*>(tile);
        if (landedProperty) {
            // Auto-flow setelah mendarat: BANK -> BELI/LELANG, OWNED -> BAYAR_SEWA otomatis.
            string landingInfo = gc->processLanding(*player, tile);
            if (!landingInfo.empty()) gv->showMessage(landingInfo);

            if (landedProperty->getStatus() == StatusType::BANK) {
                pendingProperty = landedProperty;
                pendingPlayer = player->getUsername();
                pendingNeedsDecision = true;
                pendingExtraTurn = extraTurn;
                gv->showMessage("Ketik BELI untuk membeli properti ini, atau LELANG untuk memulai lelang.");
            }
        }
    }

    if (!wentToJail) {
        bool getsExtraTurn = handlePostMove(player, gv, scm, extraTurn);
        if (pendingNeedsDecision) {
            // Turn diselesaikan setelah BELI/LELANG.
            pendingExtraTurn = getsExtraTurn;
            return;
        }
        finalizeLandingResolution(gc, gv, player, getsExtraTurn);
    } else {
        advanceTurnAndCheck(gc, gv);
    }
}

void CommandProcessor::executeCommand(const string& cmd) {
    stringstream ss(cmd);
    string token;
    ss >> token;

    if (token == "CETAK_PAPAN") {
        if (gameController != nullptr && boardView != nullptr) {
            boardView->showBoard(gameController->getBoard(), gameController->getPlayers());
        } else {
            cout << "Error: GameController atau BoardView tidak diinisialisasi." << endl;
        }
    }
    else if (token == "LEMPAR_DADU") {
        if (!gameView || !gameController) return;

        auto player = gameController->getBoard().getCurrentPlayer();
        if (!player) {
            gameView->showError("Tidak ada pemain aktif.");
            return;
        }

        gameView->showDiceRolling();
        dice.rollRandom();
        gameView->showDiceResult(dice.getDie1(), dice.getDie2());

        executeDiceMove(player.get(), dice, gameController, gameView,
                pendingProperty, pendingPlayer, pendingNeedsDecision, pendingExtraTurn);
    }
    else if (token == "ATUR_DADU") {
        if (!gameView || !gameController) return;

        int d1, d2;
        if (!(ss >> d1 >> d2)) {
            gameView->showInvalidDice();
            return;
        }

        if (d1 < 1 || d1 > 6 || d2 < 1 || d2 > 6) {
            gameView->showInvalidDice();
            return;
        }

        auto player = gameController->getBoard().getCurrentPlayer();
        if (!player) {
            gameView->showError("Tidak ada pemain aktif.");
            return;
        }

        dice.setManual(d1, d2);
        int total = dice.getTotal();

        int sz = static_cast<int>(gameController->getBoard().getTiles().size());
        int previewPos = sz > 0 ? (player->getPosition() + total) % sz : 0;
        Tile* destTile = gameController->getBoard().getTileAt(previewPos);
        string destination = destTile ? destTile->getName() : "Unknown";
        gameView->showDiceSetDetailed(player->getUsername(), d1, d2, destination);

        executeDiceMove(player.get(), dice, gameController, gameView,
                pendingProperty, pendingPlayer, pendingNeedsDecision, pendingExtraTurn);
    }
    else if (token == "BELI") {
        if (!gameView || !gameController) return;
        auto current = gameController->getBoard().getCurrentPlayer();
        if (!current) { gameView->showError("Tidak ada pemain aktif."); return; }

        if (!pendingNeedsDecision || !pendingProperty || current->getUsername() != pendingPlayer) {
            gameView->showError("Tidak ada properti yang menunggu keputusan BELI.");
            return;
        }

        int moneyBefore = current->getMoney();
        gameController->processPurchase(*current, pendingProperty);
        if (pendingProperty->getOwner() == current->getUsername()) {
            gameView->showPurchaseSuccess(pendingProperty->getName(), current->getMoney());
            pendingNeedsDecision = false;
            pendingProperty = nullptr;
            pendingPlayer.clear();
            finalizeLandingResolution(gameController, gameView, current.get(), pendingExtraTurn);
            pendingExtraTurn = false;
        } else {
            if (current->getMoney() < pendingProperty->getPurchasePrice()) {
                gameView->showError("Uang tidak cukup untuk membeli. Gunakan LELANG.");
            } else if (current->getMoney() == moneyBefore) {
                gameView->showError("Pembelian gagal. Gunakan LELANG.");
            }
        }
    }
    else if (token == "LELANG") {
        if (!gameView || !gameController) return;
        auto current = gameController->getBoard().getCurrentPlayer();
        if (!current) { gameView->showError("Tidak ada pemain aktif."); return; }

        if (!pendingNeedsDecision || !pendingProperty || current->getUsername() != pendingPlayer) {
            gameView->showError("Tidak ada properti yang menunggu keputusan LELANG.");
            return;
        }

        gameController->processAuction(pendingProperty);
        if (!gameController->isAuctionActive()) {
            gameView->showError("Lelang gagal dimulai.");
            return;
        }

        auctionOrder.clear();
        for (const auto& p : gameController->getPlayers()) {
            if (p && p->getStatus() != BANKRUPT) auctionOrder.push_back(p);
        }
        if (auctionOrder.empty()) {
            gameView->showError("Tidak ada peserta lelang aktif.");
            return;
        }

        auctionIndex = 0;
        auctionRunning = true;
        gameView->showAuctionStart(pendingProperty->getName(), pendingProperty->getCode());
        gameView->showAuctionTurn(auctionOrder[auctionIndex]->getUsername());
    }
    else if (token == "BID") {
        if (!gameView || !gameController) return;
        if (!auctionRunning || !gameController->isAuctionActive()) {
            gameView->showError("Tidak ada lelang aktif.");
            return;
        }
        int amount;
        if (!(ss >> amount) || amount <= 0) {
            gameView->showError("Format: BID <jumlah>");
            return;
        }
        if (auctionOrder.empty()) {
            gameView->showError("Daftar peserta lelang kosong.");
            return;
        }

        auto actor = auctionOrder[auctionIndex];
        if (!actor || actor->getStatus() == BANKRUPT) {
            auctionIndex = (auctionIndex + 1) % static_cast<int>(auctionOrder.size());
            gameView->showAuctionTurn(auctionOrder[auctionIndex]->getUsername());
            return;
        }

        bool ok = gameController->placeAuctionBid(actor->getUsername(), amount);
        if (!ok) {
            gameView->showError("Bid tidak valid. Pastikan lebih tinggi dari bid saat ini dan uang mencukupi.");
            return;
        }

        gameView->showHighestBid(gameController->getAuctionCurrentBid(), gameController->getAuctionCurrentBidderName());
        if (gameController->isAuctionEnded()) {
            gameView->showMessage("Kondisi akhir lelang tercapai. Ketik SELESAI_LELANG.");
            return;
        }

        auctionIndex = (auctionIndex + 1) % static_cast<int>(auctionOrder.size());
        gameView->showAuctionTurn(auctionOrder[auctionIndex]->getUsername());
    }
    else if (token == "PASS") {
        if (!gameView || !gameController) return;
        if (!auctionRunning || !gameController->isAuctionActive()) {
            gameView->showError("Tidak ada lelang aktif.");
            return;
        }
        if (auctionOrder.empty()) {
            gameView->showError("Daftar peserta lelang kosong.");
            return;
        }

        auto actor = auctionOrder[auctionIndex];
        if (actor && actor->getStatus() != BANKRUPT) {
            gameController->passAuction(actor->getUsername());
        }

        if (gameController->isAuctionEnded()) {
            gameView->showMessage("Kondisi akhir lelang tercapai. Ketik SELESAI_LELANG.");
            return;
        }

        auctionIndex = (auctionIndex + 1) % static_cast<int>(auctionOrder.size());
        gameView->showAuctionTurn(auctionOrder[auctionIndex]->getUsername());
    }
    else if (token == "SELESAI_LELANG") {
        if (!gameView || !gameController) return;
        if (!auctionRunning || !gameController->isAuctionActive()) {
            gameView->showError("Tidak ada lelang aktif.");
            return;
        }
        if (!gameController->isAuctionEnded()) {
            gameView->showError("Lelang belum memenuhi kondisi selesai.");
            return;
        }

        string winnerName = gameController->getAuctionCurrentBidderName();
        int finalBid = gameController->getAuctionCurrentBid();
        bool finalized = gameController->finalizeAuction();
        if (!finalized) {
            gameView->showError("Gagal menyelesaikan lelang.");
            return;
        }

        if (!winnerName.empty()) {
            gameView->showAuctionWinner(winnerName, finalBid);
            if (pendingProperty) {
                gameView->showAuctionResult(pendingProperty->getName(), winnerName);
            }
        } else {
            gameView->showMessage("Lelang selesai tanpa pemenang.");
        }

        auto current = gameController->getBoard().getCurrentPlayer();
        pendingNeedsDecision = false;
        pendingProperty = nullptr;
        pendingPlayer.clear();
        auctionOrder.clear();
        auctionIndex = 0;
        auctionRunning = false;

        if (current) {
            finalizeLandingResolution(gameController, gameView, current.get(), pendingExtraTurn);
        }
        pendingExtraTurn = false;
    }
    else if (token == "CETAK_AKTA") {
        // CETAK_AKTA <kode_properti>
        if (!gameView || !gameController || !propertyView) return;
        string code;
        ss >> code;
        if (code.empty()) {
            gameView->showError("Format: CETAK_AKTA <kode_properti>");
            return;
        }
        const auto& tiles = gameController->getBoard().getTiles();
        Property* found = nullptr;
        for (const auto& t : tiles) {
            Property* p = dynamic_cast<Property*>(t.get());
            if (p && p->getCode() == code) { found = p; break; }
        }
        if (!found) {
            gameView->showError("Properti dengan kode '" + code + "' tidak ditemukan.");
            return;
        }
        propertyView->showPropertyDetail(found);
    }
    else if (token == "CETAK_PROPERTI") {
        // CETAK_PROPERTI [username] — jika kosong, tampilkan pemain saat ini
        if (!gameView || !gameController || !propertyView) return;
        string username;
        ss >> username;
        shared_ptr<Player> target;
        if (username.empty()) {
            target = gameController->getBoard().getCurrentPlayer();
        } else {
            target = gameController->getPlayerByUsername(username);
        }
        if (!target) {
            gameView->showError("Pemain tidak ditemukan.");
            return;
        }
        propertyView->showPlayerProperties(target.get(), gameController->getBoard().getTiles());
    }
    else if (token == "GADAI") {
        // GADAI — tampilkan properti yang bisa digadai, minta pilihan
        if (!gameView || !gameController) return;
        auto player = gameController->getBoard().getCurrentPlayer();
        if (!player) { gameView->showError("Tidak ada pemain aktif."); return; }

        // Kumpulkan properti yang dimiliki dan belum digadai
        vector<Property*> mortgageable;
        for (const auto& t : gameController->getBoard().getTiles()) {
            Property* p = dynamic_cast<Property*>(t.get());
            if (p && p->getOwner() == player->getUsername() && p->getStatus() == StatusType::OWNED) {
                mortgageable.push_back(p);
            }
        }

        if (mortgageable.empty()) {
            gameView->showNoMortgageable();
            return;
        }

        gameView->showMortgageableList(mortgageable);
        int choice = gameView->askPropertyChoice();
        if (choice < 1 || choice > static_cast<int>(mortgageable.size())) return;

        Property* prop = mortgageable[choice - 1];

        // Jika properti adalah Street dengan bangunan, tawarkan jual bangunan dulu
        Street* street = dynamic_cast<Street*>(prop);
        BuildingManager bm;
        if (street && street->getBuildingCount() != "0") {
            vector<Property*> group = { prop };
            gameView->showBuildingList(group);
            bool sell = gameView->askSellBuildings(prop->getName());
            if (sell) {
                if (street->getBuildingCount() == "H") {
                    bm.sellHotel(street, player.get());
                    gameView->showSellBuilding(street->getName(), street->getHotelPrice() / 2, player->getMoney());
                } else {
                    bm.sellHouse(street, player.get());
                    gameView->showSellBuilding(street->getName(), street->getHousePrice() / 2, player->getMoney());
                }
            }
            bool cont = gameView->askContinueMortgage(prop->getName());
            if (!cont) return;
        }

        bool ok = gameController->mortgage(*player, prop);
        if (ok) {
            gameView->showMortgageSuccess(prop->getName(), prop->getMortgageValue(), player->getMoney());
        } else {
            gameView->showCannotMortgage(prop->getName(), "");
        }
    }
    else if (token == "TEBUS") {
        // TEBUS — tampilkan properti yang digadai, minta pilihan
        if (!gameView || !gameController) return;
        auto player = gameController->getBoard().getCurrentPlayer();
        if (!player) { gameView->showError("Tidak ada pemain aktif."); return; }

        vector<Property*> mortgaged;
        for (const auto& t : gameController->getBoard().getTiles()) {
            Property* p = dynamic_cast<Property*>(t.get());
            if (p && p->getOwner() == player->getUsername() && p->getStatus() == StatusType::MORTGAGED) {
                mortgaged.push_back(p);
            }
        }

        if (mortgaged.empty()) {
            gameView->showNoMortgaged();
            return;
        }

        gameView->showMortgagedList(mortgaged);
        int choice = gameView->askRedeemChoice();
        if (choice < 1 || choice > static_cast<int>(mortgaged.size())) return;

        Property* prop = mortgaged[choice - 1];
        int cost = prop->getPurchasePrice();

        bool ok = gameController->redeem(*player, prop);
        if (ok) {
            gameView->showRedeemSuccess(prop->getName(), cost, player->getMoney());
        } else {
            gameView->showRedeemFailure(prop->getName(), cost, player->getMoney());
        }
    }
    else if (token == "BANGUN") {
        // BANGUN — tampilkan grup warna yang bisa dibangun, minta pilihan
        if (!gameView || !gameController) return;
        auto player = gameController->getBoard().getCurrentPlayer();
        if (!player) { gameView->showError("Tidak ada pemain aktif."); return; }

        // Kumpulkan semua Street milik player, kelompokkan per warna
        map<int, vector<Street*>> byColor;
        for (const auto& t : gameController->getBoard().getTiles()) {
            Street* s = dynamic_cast<Street*>(t.get());
            if (s && s->getOwner() == player->getUsername() && s->getStatus() == StatusType::OWNED) {
                byColor[static_cast<int>(s->getColorGroup())].push_back(s);
            }
        }

        // Hitung total Street per warna di board untuk cek monopoli
        map<int, int> totalPerColor;
        for (const auto& t : gameController->getBoard().getTiles()) {
            Street* s = dynamic_cast<Street*>(t.get());
            if (s) totalPerColor[static_cast<int>(s->getColorGroup())]++;
        }

        // Grup buildable: player punya semua street dalam warna itu
        vector<vector<Street*>> buildableGroups;
        for (auto& [color, streets] : byColor) {
            if (static_cast<int>(streets.size()) == totalPerColor[color]) {
                buildableGroups.push_back(streets);
            }
        }

        if (buildableGroups.empty()) {
            gameView->showNoBuildableGroup();
            return;
        }

        gameView->showBuildableGroups(buildableGroups);
        int groupChoice = gameView->askGroupChoice();
        if (groupChoice < 1 || groupChoice > static_cast<int>(buildableGroups.size())) return;

        vector<Street*>& group = buildableGroups[groupChoice - 1];
        gameView->showBuildingState(group);

        BuildingManager bm;
        // Cari street dengan rumah paling sedikit (distribusi merata)
        Street* target = nullptr;
        int minCount = INT_MAX;
        for (Street* s : group) {
            int cnt = (s->getBuildingCount() == "H") ? 5 : stoi(s->getBuildingCount());
            if (cnt < minCount) { minCount = cnt; target = s; }
        }
        if (!target) return;

        bool isHotel = (target->getBuildingCount() == "4");
        if (isHotel) {
            int cost = target->getHotelPrice();
            bool confirm = gameView->askUpgradeHotel(cost);
            if (!confirm) return;
            if (bm.buildHotel(target, player.get())) {
                gameView->showUpgradeSuccess(target->getName(), cost, player->getMoney());
                gameController->logAction(player->getUsername(), "BANGUN_HOTEL", target->getName());
            } else {
                gameView->showError("Tidak bisa membangun hotel.");
            }
        } else {
            int cost = target->getHousePrice();
            if (bm.buildHouse(target, player.get())) {
                gameView->showBuildSuccess(target->getName(), cost, player->getMoney());
                gameController->logAction(player->getUsername(), "BANGUN_RUMAH", target->getName());
            } else {
                gameView->showError("Tidak bisa membangun rumah. Cek uang atau status properti.");
            }
        }
    }
    else if (token == "SIMPAN") {
        string fname;
        ss >> fname;
        if (fname.empty()) fname = "savegame.txt";
        if (gameController != nullptr) gameController->saveGame(fname);
    }
    else if (token == "MUAT") {
        string fname;
        ss >> fname;
        if (fname.empty()) fname = "savegame.txt";
        if (gameController != nullptr) gameController->loadGame(fname);
    }
    else if (token == "CETAK_LOG") {
        if (gameController != nullptr && gameController->getLogger() != nullptr) {
            int recentCount = 0;
            if (ss >> recentCount) {
                if (recentCount < 0) {
                    gameView->showError("Format: CETAK_LOG [N], dengan N >= 0.");
                    return;
                }
            }

            vector<string> entries = (recentCount > 0)
                ? gameController->getLogger()->getRecent(recentCount)
                : gameController->getLogger()->getAll();

            cout << "[LOG] " << entries.size() << " entri:" << endl;
            for (size_t i = 0; i < entries.size(); ++i) cout << entries[i] << endl;
        }
    }
    else if (token == "MENANG") {
        if (!gameController || !gameView) return;

        if (!gameController->checkGameEnd()) {
            gameView->showMessage("Game belum selesai.");
            return;
        }

        vector<Player*> winners = gameController->getWinners();
        if (winners.empty()) {
            gameView->showMessage("Belum ada pemenang.");
            return;
        }

        if (winners.size() == 1) {
            gameView->showMessage("Pemenang: " + winners.front()->getUsername());
            return;
        }

        string line = "Pemenang bersama: ";
        for (size_t i = 0; i < winners.size(); ++i) {
            if (i > 0) line += ", ";
            line += winners[i]->getUsername();
        }
        gameView->showMessage(line);
    }
    else if (token == "GUNAKAN_KEMAMPUAN") {
        // GUNAKAN_KEMAMPUAN tampilkan kartu di tangan, minta pilihan
        if (!gameView || !gameController) return;
        auto player = gameController->getBoard().getCurrentPlayer();
        if (!player) { gameView->showError("Tidak ada pemain aktif."); return; }

        if (player->hasRolled()) {
            gameView->showSkillAfterDiceError();
            return;
        }
        if (player->hasUsedCard()) {
            gameView->showSkillAlreadyUsed();
            return;
        }

        const auto& hand = player->getHand();
        if (hand.empty()) {
            gameView->showError("Tidak ada kartu kemampuan di tangan.");
            return;
        }

        vector<string> cardNames;
        for (SkillCard* c : hand) cardNames.push_back(c->getDescription());
        gameView->showSkillCards(cardNames);
        int choice = gameView->askSkillChoice(static_cast<int>(hand.size()));
        if (choice < 1 || choice > static_cast<int>(hand.size())) return;

        SkillCard* card = hand[choice - 1];
        string cardDesc = card->getDescription();

        SkillCardManager* scm = gameController->getSkillCardManager();
        if (!scm) return;

        // GameContext dibutuhkan oleh card->activate()
        // Akses via MovementHandler yang sudah punya pointer ke context
        GameContext* ctx = nullptr;
        MovementHandler* mh = gameController->getMovementHandler();
        if (mh) ctx = mh->getContext();

        scm->useCard(player.get(), choice - 1, ctx);
        gameView->showSkillActivated(cardDesc, cardDesc);
        gameController->logAction(player->getUsername(), "GUNAKAN_KEMAMPUAN", cardDesc);
    }
    else if (token == "EXIT") {
        cout << "[INFO] Game ditutup." << endl;
        exit(0);
    }
    else if (token == "HELP") {
        cout << "Commands tersedia:\n";
        cout << "  CETAK_PAPAN     - Menampilkan board game\n";
        cout << "  CETAK_PROPERTI  - Menampilkan detail properti\n";
        cout << "  LEMPAR_DADU     - Melempar dadu secara random\n";
        cout << "  ATUR_DADU [X Y] - Atur hasil dadu secara manual\n";
        cout << "  BELI            - Beli properti yang baru diinjak\n";
        cout << "  LELANG          - Mulai lelang properti yang baru diinjak\n";
        cout << "  BID <jumlah>    - Menawar saat lelang aktif\n";
        cout << "  PASS            - Lewat saat lelang aktif\n";
        cout << "  SELESAI_LELANG  - Menutup lelang jika kondisi selesai terpenuhi\n";
        cout << "  GADAI           - Menggadaikan properti\n";
        cout << "  TEBUS           - Menebus properti yang digadai\n";
        cout << "  BANGUN          - Membangun rumah/hotel di properti\n";
        cout << "  SIMPAN [file]   - Simpan state game (default savegame.txt)\n";
        cout << "  MUAT   [file]   - Muat state game (default savegame.txt)\n";
        cout << "  CETAK_LOG [N]   - Tampilkan seluruh / N log transaksi terbaru\n";
        cout << "  MENANG          - Tampilkan pemenang saat game berakhir\n";
        cout << "  EXIT            - Keluar dari game\n";
        cout << "  HELP            - Tampilkan help ini\n";
    }
    else {
        cout << "Perintah '" << cmd << "' tidak dikenali. Ketik HELP untuk bantuan." << endl;
    }
}
