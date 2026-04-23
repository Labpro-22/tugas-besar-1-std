#ifndef ACTIONCARD_H
#define ACTIONCARD_H

#pragma once
#include <string>

class Player;
class GameContext;  // satu context

enum class ActionCardType {
    GAIN_MONEY,
    PAY_MONEY,
    COLLECT_FROM_ALL,
    PAY_TO_ALL,
    MOVE,
    TELEPORT,
    GO_TO_JAIL,
    GET_SKILL
};

class ActionCard {
public:
    virtual void execute(Player* player, GameContext* ctx) = 0;
    virtual std::string getDescription() = 0;
    virtual std::string getCardType() const = 0;
    virtual ~ActionCard() {}
};

class ChanceCard : public ActionCard {
private:
    ActionCardType type;
    int value;
    std::string desc;

public:
    ChanceCard(ActionCardType t, int v, std::string d);
    void execute(Player* player, GameContext* ctx) override;
    std::string getDescription() override;
    std::string getCardType() const override;
    ActionCardType getType() const;
    int getValue() const;
};

class CommunityCard : public ActionCard {
private:
    ActionCardType type;
    int value;
    std::string desc;

public:
    CommunityCard(ActionCardType t, int v, std::string d);
    void execute(Player* player, GameContext* ctx) override;
    std::string getDescription() override;
    std::string getCardType() const override;
    ActionCardType getType() const;
    int getValue() const;
};

#endif
