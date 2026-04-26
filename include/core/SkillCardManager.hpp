#ifndef SKILLCARDMANAGER_H
#define SKILLCARDMANAGER_H

#include <vector>
#include <cstdlib>
#include <stdexcept>

#include "../utils/CardDeck.hpp"
#include "../utils/SkillCard.hpp"
#include <cstdlib>
#include <stdexcept>

using namespace std;

// Forward declarations
class Player;
class GameContext;
class GameBoard;

class SkillCardManager {
private:
    CardDeck<SkillCard>* skillDeck;
    int maxHandSize;

    bool isValidIndex(Player* player, int idx) const;

public:
    explicit SkillCardManager(GameBoard& board, int maxSize = 3);

    void initDeck();

    void distributeCardToAll(const std::vector<Player*>& players);

    SkillCard* distributeCardTo(Player* player);

    void useCard(Player* player, int idx, GameContext* ctx);

    void dropCard(Player* player, int idx);

    void decrementDurations(Player* player);

    bool isHandOverflow(Player* player) const;

    int getMaxHandSize() const { return maxHandSize; }

    CardDeck<SkillCard>& getDeck() {
        return *skillDeck;
    }

    const CardDeck<SkillCard>& getDeck() const { return *skillDeck; }
};

#endif
