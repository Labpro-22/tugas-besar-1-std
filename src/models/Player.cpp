#include "Player.hpp"
#include "SkillCard.hpp"
#include "GameBoard.hpp"
#include "Property.hpp"
#include "Street.hpp"

Player::Player(const string& name, int startMoney)
    : username(name), money(startMoney), position(0),
      status(ACTIVE), jailTurnsRemaining(0), consecutiveDoubles(0),
      hasUsedCardThisTurn(false), hasRolledThisTurn(false),
      shieldActive(false), discountPercent(0) {}

Player::~Player() {
    hand.clear();
}

// ---- Getters ----
string Player::getUsername() const { return username; }
int Player::getMoney() const { return money; }
int Player::getPosition() const { return position; }
PlayerStatus Player::getStatus() const { return status; }
vector<SkillCard*>& Player::getHand() { return hand; }
const vector<SkillCard*>& Player::getHand() const { return hand; }
bool Player::hasRolled() const { return hasRolledThisTurn; }
bool Player::hasUsedCard() const { return hasUsedCardThisTurn; }
bool Player::isShieldActive() const { return shieldActive; }
int Player::getDiscountPercent() const { return discountPercent; }

//  Setters 
void Player::setPosition(int pos) { position = pos; }
void Player::setStatus(PlayerStatus s) { status = s; }

//  Money operations 

// Pembayaran sukarela (beli properti, lelang, bangun, tebus)
void Player::payVoluntary(int amt) {
    if (amt > 0) money -= amt;
}

// Terima uang
Player& Player::operator+=(int amt) {
    if (amt > 0) money += amt;
    return *this;
}

// Pembayaran paksa (sewa, pajak, denda, efek kartu negatif)
Player& Player::operator-=(int amt) {
    if (amt <= 0) return *this;

    if (shieldActive) {
        shieldActive = false; 
        return *this;
    }

    money -= amt;
    return *this;
}

// calculateTotalWealth (hanya uang tunai) 
int Player::calculateTotalWealth() const {
    return money;
}

//  Operator overloading
bool Player::operator>(const Player& other) const { return money > other.money; }
bool Player::operator<(const Player& other) const { return money < other.money; }

//  Consecutive doubles 
void Player::incrementConsecutiveDoubles() { consecutiveDoubles++; }
void Player::resetConsecutiveDoubles() { consecutiveDoubles = 0; }
int Player::getConsecutiveDoubles() const { return consecutiveDoubles; }

//  Jail turn tracking 
// giliran ke 4 wajib bayar denda
void Player::incrementJailTurns() { jailTurnsRemaining++; }
void Player::resetJailTurns() { jailTurnsRemaining = 0; }
int Player::getJailTurnsRemaining() const { return jailTurnsRemaining; }

//  Turn flags 
void Player::markRolled() { hasRolledThisTurn = true; }
void Player::markCardUsed() { hasUsedCardThisTurn = true; }

// Dipanggil TurnManager::resetTurnFlags di awal giliran.
// Shield dan discount tidak ke giliran berikutnya.
void Player::resetTurnFlags() {
    hasRolledThisTurn = false;
    hasUsedCardThisTurn = false;
    shieldActive = false;
    discountPercent = 0;
}

// Card hand management 
void Player::receiveCard(SkillCard* c) {
    if (c) hand.push_back(c);
}

// Keluarkan kartu dari hand 
SkillCard* Player::removeCard(int idx) {
    if (idx < 0 || idx >= static_cast<int>(hand.size())) return nullptr;
    SkillCard* c = hand[idx];
    hand.erase(hand.begin() + idx);
    return c;
}

// Active effects 
void Player::activateShield() { shieldActive = true; }
void Player::clearShield() { shieldActive = false; }
void Player::setDiscount(int d) { discountPercent = d; }
void Player::clearDiscount() { discountPercent = 0; }