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
    void readPlayerStates(std::ifstream& in, GameBoard* board);
    void readTurnOrder(std::ifstream& in, GameBoard* board);
    void readPropertyStates(std::ifstream& in, GameBoard* board);
    std::vector<SkillCard*> readDeckState(std::ifstream& in);
    void readLogState(std::ifstream& in, TransactionLogger* logger);

public:
    bool loadSave(const std::string& filename,
                  GameBoard* board,
                  TransactionLogger* logger);
    bool validate(const std::string& filename);
};

#endif
