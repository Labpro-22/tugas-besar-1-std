#ifndef SKILLCARD_H
#define SKILLCARD_H

#include <string>
using namespace std;

class Player;
class GameContext;

class SkillCard {
protected:
    string cardType;
    int value;
    int remainingDuration;

public:
    SkillCard(string type, int val, int duration);
    virtual ~SkillCard() {}

    virtual void activate(Player* player, GameContext* ctx) = 0;
    virtual string getDescription() = 0;

    void decrementDuration();

    string getCardType() const { return cardType; }
    int getValue() const { return value; }
    int getRemainingDuration() const { return remainingDuration; }
    void setRemainingDuration(int d) { remainingDuration = d; }
};

// Turunan Skillcard

class MoveCard : public SkillCard {
public:
    MoveCard(int steps);
    void activate(Player* player, GameContext* ctx) override;
    string getDescription() override;
};

class ShieldCard : public SkillCard {
public:
    ShieldCard();
    void activate(Player* player, GameContext* ctx) override;
    string getDescription() override;
};

class TeleportCard : public SkillCard {
public:
    TeleportCard(int target);
    void activate(Player* player, GameContext* ctx) override;
    string getDescription() override;
};

class DiscountCard : public SkillCard {
public:
    DiscountCard(int percent);
    void activate(Player* player, GameContext* ctx) override;
    string getDescription() override;
};

class LassoCard : public SkillCard {
public:
    LassoCard();
    void activate(Player* player, GameContext* ctx) override;
    string getDescription() override;
};

class DemolitionCard : public SkillCard {
public:
    DemolitionCard();
    void activate(Player* player, GameContext* ctx) override;
    string getDescription() override;
};

#endif
