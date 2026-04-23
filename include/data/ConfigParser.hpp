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
    std::string basePath;

    std::map<int, int> railroadRentTable;
    std::map<int, int> utilityMultiplierTable;
    int pphFlat;
    int pphPercentage;
    int pbmFlat;
    int goSalary;
    int jailFine;
    int maxTurn;
    int startBalance;

    CardDeck<ActionCard>* chanceDeck;
    CardDeck<ActionCard>* communityDeck;

    std::map<int, Tile*> stagedTiles;

    void parsePropertyConfig(const std::string& filename);
    void parseRailroadConfig(const std::string& filename);
    void parseUtilityConfig(const std::string& filename);
    void parseTaxConfig(const std::string& filename);
    void parseActionConfig(const std::string& filename);
    void parseSpecialConfig(const std::string& filename);
    void parseMiscConfig(const std::string& filename);

public:
    explicit ConfigParser(const std::string& basePath);
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
