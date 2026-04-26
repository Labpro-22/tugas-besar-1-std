#ifndef TURNMANAGER_H
#define TURNMANAGER_H

#include <vector>
#include <string>
using namespace std;

class Player;

class TurnManager {
private:
    vector<Player*> players;
    vector<string> turnOrder;
    int currentIndex;
    int currentTurnNumber;
    int maxTurn;

public:
    TurnManager(vector<Player*> p, int maxT);

    Player* getCurrentPlayer();
    void advanceToNextPlayer();

    int getCurrentTurnNumber();
    int getMaxTurn();
    bool isMaxTurnReached();
    vector<string> getTurnOrder();
    void setTurnOrder(vector<string> order);

    void resetTurnFlags(Player* p);
};

#endif
