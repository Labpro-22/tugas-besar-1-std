#ifndef GAME_VIEW_HPP
#define GAME_VIEW_HPP

#include <string>
#include <vector>
#include "../../include/models/Player.hpp"
#include "../../include/models/Property.hpp"
#include "../../include/models/Street.hpp"

class GameView {
public:
    void showMessage(const std::string& msg);
    void showError(const std::string& msg);
    void showHeader();
    void showTurnInfo(int currentTurn, int maxTurn);
    void showDiceSetDetailed(const std::string& playerName, int d1, int d2, const std::string& destination);
    void showInvalidDice();
    void showPlayerTurn(const std::string& username);
    void showCommandList();

    void showDiceRolling();
    void showDiceResult(int d1, int d2);
    void showPlayerMove(const std::string& playerName, int steps);
    void showLanding(const std::string& tileName);

    //beli
    void showLandingProperty(const std::string& name, const std::string& code);
    void showPropertySimple(const std::string& color, const std::string& name, const std::string& code, int price, int rent);

    void showPlayerMoney(int money);
    bool askBuyProperty(int price);

    void showPurchaseSuccess(const std::string& name, int remainingMoney);
    void showAuctionStart();
    void showAutoOwned(const std::string& name);

    // sewa
    void showRentPayment(const std::string& propertyName,const std::string& code, const std::string& ownerName, int rent,int playerMoneyBefore, int playerMoneyAfter, int ownerMoneyBefore, int ownerMoneyAfter);
    void showMortgagedProperty(const std::string& propertyName, const std::string& code, const std::string& ownerName);
    void showCannotPayRent(int rent, int playerMoney);

    // gadai
    void showMortgageableList(const vector<Property*>& props);
    int askPropertyChoice();

    void showMortgageSuccess(const string& name, int value, int money);
    void showNoMortgageable();

    void showCannotMortgage(const string& name, const string& color);
    void showBuildingList(const vector<Property*>& group);

    bool askSellBuildings(const string& color);
    void showSellBuilding(const string& name, int value, int money);

    bool askContinueMortgage(const string& name);

    // tebus
    void showMortgagedList(const vector<Property*>& props);
    int askRedeemChoice();
    void showRedeemSuccess(const string& name, int cost, int money);
    void showRedeemFailure(const string& name, int cost, int money);
    void showNoMortgaged();

    // bangun
    void showBuildableGroups(const vector<vector<Street*>>& groups);
    int askGroupChoice();
    bool askUpgradeHotel(int cost);
    void showNoBuildableGroup();
    void showBuildSuccess(const string& name, int cost, int money);
    void showUpgradeSuccess(const string& name, int cost, int money);
    void showBuildingState(const vector<Street*>& group);

    // lelang
    void showAuctionStart(const string& name, const string& code);
    void showAuctionTurn(const string& playerName);
    string askAuctionAction();
    void showHighestBid(int amount, const string& playerName);
    void showAuctionWinner(const string& playerName, int amount);
    void showAuctionResult(const string& propertyName, const string& winnerName);
    
    // festival
    void showFestivalStart();
    void showOwnedProperties(const vector<Property*>& props);
    string askPropertyCode();
    void showFestivalActivated(int oldRent, int newRent);
    void showFestivalBoosted(int oldRent, int newRent);
    void showFestivalMax(int currentRent);
    void showFestivalInvalidCode();
    void showFestivalNotOwned();

    // kebangkrutan
    void showDebt(const string& creditor, int money, int debt);
    void showLiquidationEstimate(const vector<string>& items, int total);
    void showLiquidationPanel(int money, int debt, const vector<string>& sellList, const vector<string>& mortgageList);
    void showBankrupt(const string& player, const string& creditor);
    void showBankruptcyBank(int money);
    
    void showDebtPaid(int debt, const string& creditor, int playerBefore, int playerAfter, int creditorBefore, int creditorAfter);
    void showAssetTransferToPlayer(const string& creditor,int money, const vector<string>& assets);
    void showPlayerEliminated(const string& player, int remaining);
    void showAuctionBatchStart();
    void showAuctionItem(const string& name, const string& code);

    void showTotalAsset(int totalAsset);
    void showInsufficientTotal(int debt);
    void showAuctionNote();

    // kesempatan atau dana umum
    void showChanceTile();
    void showCommunityChestTile();
    void showDrawCard();
    void showCardText(const string& text);
    void showCardMove(const string& code);
    void showPayToBank(int amount, int remainingMoney);
    void showCannotPayCard(const string& reason, int amount, int money);

    //gunakan kemampuan
    void showSkillCards(const vector<string>& cards);
    int askSkillChoice(int maxChoice);
    void showSkillActivated(const string& name, const string& desc);
    void showSkillAlreadyUsed();
    void showSkillAfterDiceError();
    void showShieldBlock(int amount, int money);

    // drop kartu
    void showNewSkillCard(const string& cardName);
    void showSkillOverflowWarning();
    void showSkillCardsWithNew(const vector<string>& cards);
    int askDiscardSkill(int maxChoice);
    void showSkillDiscarded(const string& cardName, int remaining);
};

#endif