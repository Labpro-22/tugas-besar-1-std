#ifndef GAME_LOADER_HPP
#define GAME_LOADER_HPP

#include "FileParser.hpp"

#include <fstream>
#include <string>
#include <vector>

using namespace std;


class GameBoard;
class SkillCard;
class TransactionLogger;

class GameLoader : public FileParser {
private:
    void readPlayerStates(ifstream& in, GameBoard* board);
    void readTurnOrder(ifstream& in, GameBoard* board);
    void readPropertyStates(ifstream& in, GameBoard* board);
    vector<SkillCard*> readDeckState(ifstream& in);
    void readLogState(ifstream& in, TransactionLogger* logger);

public:
    bool loadSave(const string& filename,
                  GameBoard* board,
                  TransactionLogger* logger);
    bool validate(const string& filename);
};

#endif
