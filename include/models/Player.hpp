#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
using namespace std;

class SkillCard;

enum PlayerStatus {
    ACTIVE,
    BANKRUPT,
    JAILED
};

class Player {
private:
    string username;
    int money;
    int position;
    PlayerStatus status;
    int jailTurnsRemaining;
    int consecutiveDoubles;
    vector<SkillCard*> hand; // pointer sementara, CardDeck yang owner
    bool hasUsedCardThisTurn;
    bool hasRolledThisTurn;
    bool shieldActive;
    int discountPercent;

public:
    Player(string name, int startMoney);
    ~Player();

    // getter
    string getUsername();
    int getMoney();
    int getPosition();
    PlayerStatus getStatus();
    vector<SkillCard*>& getHand();
    bool hasRolled();
    bool hasUsedCard();
    bool isShieldActive();
    int getDiscountPercent();

    // setter
    void setPosition(int pos);
    void setStatus(PlayerStatus s);

    // money
    void payVoluntary(int amt);   // beli properti, lelang — tidak kena shield
    Player& operator+=(int amt);  // terima uang
    Player& operator-=(int amt);  // kena tagihan/sanksi — kena shield

    // wealth — untuk WinConditionChecker dan PPH
    int calculateTotalWealth() const;

    // comparison — untuk WinConditionChecker MAX_TURN
    bool operator>(const Player& other) const;
    bool operator<(const Player& other) const;

    // double
    void incrementConsecutiveDoubles();
    void resetConsecutiveDoubles();
    int getConsecutiveDoubles();

    // jail
    void incrementJailTurns();
    void resetJailTurns();
    int getJailTurnsRemaining();

    // turn
    void markRolled();
    void markCardUsed();
    void resetTurnFlags();

    // card — CardDeck yang owner, hand hanya pegang sementara
    void receiveCard(SkillCard* c);
    SkillCard* removeCard(int idx);

    // effect
    void activateShield();
    void clearShield();
    void setDiscount(int d);
    void clearDiscount();
};

#endif