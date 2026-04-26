#ifndef ACTIONCARD_H
#define ACTIONCARD_H

#include <string>
#include <string>
#include <climits>

using namespace std;

// Forward declarations
class Player;
class GameContext;


enum class ActionCardType {
    GAIN_MONEY,        
    PAY_MONEY,         
    COLLECT_FROM_ALL,  
    PAY_TO_ALL,        
    NEAREST_RAILROAD,  
    MOVE_BACKWARD,     
    GO_TO_JAIL         
};

// Abstract 
class ActionCard {
public:
    virtual void execute(Player* player, GameContext* ctx) = 0;
    virtual std::string getDescription() = 0;
    virtual std::string getCardType() const = 0;
    virtual ~ActionCard() {}
};

// ChanceCard: Kartu Kesempatan
class ChanceCard : public ActionCard {
private:
    ActionCardType type;
    int value;
    std::string desc;

public:
    ChanceCard(ActionCardType t, int v, std::string d);
    void execute(Player* player, GameContext* ctx) override;
    std::string getDescription() override;
    std::string getCardType() const override { return "ChanceCard"; }
    ActionCardType getType() const { return type; }
    int getValue() const { return value; }
};

// CommunityCard: Kartu Dana Umum
class CommunityCard : public ActionCard {
private:
    ActionCardType type;
    int value;
    std::string desc;

public:
    CommunityCard(ActionCardType t, int v, std::string d);
    void execute(Player* player, GameContext* ctx) override;
    std::string getDescription() override;
    std::string getCardType() const override { return "CommunityCard"; }
    ActionCardType getType() const { return type; }
    int getValue() const { return value; }
};

#endif
