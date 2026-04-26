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
    void writePlayerStates(ofstream& out, GameBoard* board) const;
    void writeTurnOrder(ofstream& out, GameBoard* board) const;
    void writePropertyStates(ofstream& out,
                             const vector<unique_ptr<Tile>>& tiles) const;
    void writeDeckState(ofstream& out,
                        const vector<SkillCard*>& deck) const;
    void writeLogState(ofstream& out,
                       TransactionLogger* logger) const;

public:
    bool save(GameBoard* board, TransactionLogger* logger,
              const string& filename) const;
};

#endif
