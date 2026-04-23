#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "FileParser.hpp"

#include <map>
#include <string>
using namespace std;

class GameBoard;
class Tile;
template <typename T> class CardDeck;
class ActionCard;

class ConfigParser : public FileParser {
private:
    string basePath;

    map<int, int> railroadRentTable;
    map<int, int> utilityMultiplierTable;
    int pphFlat;
    int pphPercentage;
    int pbmFlat;
    int goSalary;
    int jailFine;
    int maxTurn;
    int startBalance;

    CardDeck<ActionCard>* chanceDeck;
    CardDeck<ActionCard>* communityDeck;

    map<int, Tile*> stagedTiles;

    void parsePropertyConfig(const string& filename);
    void parseRailroadConfig(const string& filename);
    void parseUtilityConfig(const string& filename);
    void parseTaxConfig(const string& filename);
    void parseActionConfig(const string& filename);
    void parseSpecialConfig(const string& filename);
    void parseMiscConfig(const string& filename);

public:
    explicit ConfigParser(const string& basePath);
    ~ConfigParser();

    void loadConfig(GameBoard* board);

    int getMaxTurn() const;
    int getStartBalance() const;
    int getGoSalary() const;
    int getJailFine() const;
    int getPphFlat() const;
    int getPphPercentage() const;
    int getPbmFlat() const;
};

#endif
