#ifndef SKILLCARDMANAGER_H
#define SKILLCARDMANAGER_H

#include <vector>
#include "../utils/CardDeck.hpp"
#include "../utils/SkillCard.hpp"

using namespace std;

class Player;
class GameContext;

class SkillCardManager {
private:
    CardDeck<SkillCard> skillDeck;
    size_t maxHandSize;

    bool isValidIndex(Player* player, int idx);

public:
    SkillCardManager(int maxSize);

    void initDeck();

    void distributeCardToAll(vector<Player*> players);
    SkillCard* distributeCardTo(Player* player);
    void useCard(Player* player, int idx, GameContext* ctx);
    void dropCard(Player* player, int idx);
    void decrementDurations(Player* player);

    // Untuk save/load
    CardDeck<SkillCard>& getDeck() { return skillDeck; }
};

#endif