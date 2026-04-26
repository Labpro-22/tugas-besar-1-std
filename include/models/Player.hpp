#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
using namespace std;

//forward declaration
class SkillCard;
class GameBoard;   

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
    vector<SkillCard*> hand;
    bool hasUsedCardThisTurn;
    bool hasRolledThisTurn;
    bool shieldActive;
    int discountPercent;

public:
    Player(const string& name, int startMoney);
    ~Player();

    //  Getters 
    string getUsername() const;
    int getMoney() const;
    int getPosition() const;
    PlayerStatus getStatus() const;
    vector<SkillCard*>& getHand();
    const vector<SkillCard*>& getHand() const;
    bool hasRolled() const;
    bool hasUsedCard() const;
    bool isShieldActive() const;
    int getDiscountPercent() const;

    //  Setters 
    void setPosition(int pos);
    void setStatus(PlayerStatus s);

    //  Money operations 

    // payVoluntary: beli properti, lelang, bangun, tebus, dibikin buat bypass Shield 
    void payVoluntary(int amt);

    // operator+=: terima uang
    Player& operator+=(int amt);

    // operator-=: sewa, pajak, denda, efek kartu negatif.
    // Shield memblok sekali trus habis.
    Player& operator-=(int amt);

    // calculateTotalWealth (uang tunai aja karna player bukan responsibility akses properti) 
    int calculateTotalWealth() const;

    bool operator>(const Player& other) const;
    bool operator<(const Player& other) const;

    // Consec double
    void incrementConsecutiveDoubles();
    void resetConsecutiveDoubles();
    int getConsecutiveDoubles() const;

    // Menghitung berapa kali player gagal keluar penjara.
    // Increment: MovementHandler::handleJailTurn saat STILL_JAILED.
    // Reset: MovementHandler::sendToJail dan saat berhasil keluar.
    void incrementJailTurns();
    void resetJailTurns();
    int getJailTurnsRemaining() const;

    //  Turn flags mark 
    void markRolled();
    void markCardUsed();
    void resetTurnFlags();  // dipanggil TurnManager::resetTurnFlags di awal giliran

    //  Card hand management 
    void receiveCard(SkillCard* c);
    SkillCard* removeCard(int idx);

    // Active effects
    void activateShield();
    void clearShield();
    void setDiscount(int d);
    void clearDiscount();
};

#endif