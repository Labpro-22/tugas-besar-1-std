#include "../include/models/Player.hpp"
#include "../include/utils/SkillCard.hpp"

Player::Player(string name, int startMoney) {
    username = name;
    money = startMoney;
    position = 0;
    status = ACTIVE;
    jailTurnsRemaining = 0;
    consecutiveDoubles = 0;
    hasUsedCardThisTurn = false;
    hasRolledThisTurn = false;
    shieldActive = false;
    discountPercent = 0;
}

Player::~Player() {
    hand.clear();
}

string Player::getUsername() { return username; }
int Player::getMoney() { return money; }
int Player::getPosition() { return position; }
PlayerStatus Player::getStatus() { return status; }
vector<SkillCard*>& Player::getHand() { return hand; }
bool Player::hasRolled() { return hasRolledThisTurn; }
bool Player::hasUsedCard() { return hasUsedCardThisTurn; }
bool Player::isShieldActive() { return shieldActive; }
int Player::getDiscountPercent() { return discountPercent; }

void Player::setPosition(int pos) { position = pos; }
void Player::setStatus(PlayerStatus s) { status = s; }


// Untuk beli properti, tebus gadai, bangun, lelang tidak kena efek shield
void Player::payVoluntary(int amt) {
    money -= amt;
}

// Terima uang — selalu berlaku
Player& Player::operator+=(int amt) {
    money += amt;
    return *this;
}

// sewa, pajak, denda, efek kartu negatif bisa diblock shield
Player& Player::operator-=(int amt) {
    if (amt <= 0) {
        money -= amt;
        return *this;
    }
    if (shieldActive) {
        shieldActive = false; // habis
        return *this;
    }
    money -= amt;
    return *this;
}

// Untuk PPH persentase dan WinConditionChecker
// Total = uang tunai + harga beli properti + harga bangunan
// Property detail dihitung di TaxTile/WinConditionChecker via GameContext
// Player hanya tahu uang tunainya sendiri
int Player::calculateTotalWealth() const {
    // Hanya uang tunai — properti dihitung oleh caller via board
    return money;
}

// Untuk WinConditionChecker MAX_TURN
bool Player::operator>(const Player& other) const {
    return money > other.money;
}

bool Player::operator<(const Player& other) const {
    return money < other.money;
}

void Player::incrementConsecutiveDoubles() { consecutiveDoubles++; }
void Player::resetConsecutiveDoubles() { consecutiveDoubles = 0; }
int Player::getConsecutiveDoubles() { return consecutiveDoubles; }

void Player::incrementJailTurns() { jailTurnsRemaining++; }
void Player::resetJailTurns() { jailTurnsRemaining = 0; }
int Player::getJailTurnsRemaining() { return jailTurnsRemaining; }

void Player::markRolled() { hasRolledThisTurn = true; }
void Player::markCardUsed() { hasUsedCardThisTurn = true; }

void Player::resetTurnFlags() {
    hasRolledThisTurn = false;
    hasUsedCardThisTurn = false;
    shieldActive = false;
    discountPercent = 0;
}

void Player::receiveCard(SkillCard* c) {
    if (c != nullptr) {
        hand.push_back(c);
    }
}

SkillCard* Player::removeCard(int idx) {
    if (idx < 0 || idx >= static_cast<int>(hand.size())) return nullptr;
    SkillCard* c = hand[idx];
    hand.erase(hand.begin() + idx);
    return c; // pointer dikembalikan ke CardDeck untuk discard
}

void Player::activateShield() { shieldActive = true; }
void Player::clearShield() { shieldActive = false; }
void Player::setDiscount(int d) { discountPercent = d; }
void Player::clearDiscount() { discountPercent = 0; }