#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Tile.hpp"
#include "Player.hpp"

class Tile;
class Player;
class SkillCard;
template <typename T> class CardDeck;


class GameBoard {
private:
    // std::vector<Tile*> tiles;
    // std::vector<Player*> players;
    std::vector<std::unique_ptr<Tile>> tiles;
    std::vector<std::shared_ptr<Player>> players;
    int currentPlayerIndex;
    int currentTurnNumber = 1;
    int maxTurn = 0;
    CardDeck<SkillCard*>* skillDeck = nullptr;

public:
    GameBoard();

    
    void addTile(std::unique_ptr<Tile> tile);
    Tile* getTileAt(int position) const;

    void addPlayer(std::shared_ptr<Player> player);
    std::shared_ptr<Player> getPlayerByUsername(const std::string& username) const;
    std::shared_ptr<Player> getCurrentPlayer() const;

    void nextTurn();

    const std::vector<std::shared_ptr<Player>>& getPlayers() const;
    const std::vector<std::unique_ptr<Tile>>& getTiles() const;

    int getCurrentTurnNumber() const;
    void setCurrentTurnNumber(int t);
    int getMaxTurn() const;
    void setMaxTurn(int m);

    CardDeck<SkillCard*>* getSkillDeck() const;
    void setSkillDeck(CardDeck<SkillCard*>* d);

    void setCurrentPlayerByUsername(const std::string& username);
    void setTurnOrder(const std::vector<std::string>& order);
};
