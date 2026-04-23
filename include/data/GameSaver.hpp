#ifndef GAME_SAVER_HPP
#define GAME_SAVER_HPP

#include <fstream>
#include <memory>
#include <string>
#include <vector>

class GameBoard;
class Player;
class Tile;
class SkillCard;
class TransactionLogger;

using namespace std;


class GameSaver {
private:
    void writePlayerStates(std::ofstream& out,
                           const std::vector<std::shared_ptr<Player>>& players) const;
    void writeTurnOrder(std::ofstream& out, GameBoard* board) const;
    void writePropertyStates(std::ofstream& out,
                             const std::vector<std::unique_ptr<Tile>>& tiles) const;
    void writeDeckState(std::ofstream& out,
                        const std::vector<SkillCard*>& deck) const;
    void writeLogState(std::ofstream& out,
                       TransactionLogger* logger) const;

public:
    bool save(GameBoard* board, TransactionLogger* logger,
              const std::string& filename) const;
};

#endif
