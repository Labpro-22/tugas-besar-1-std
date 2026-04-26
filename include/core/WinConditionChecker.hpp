#ifndef WIN_CONDITION_CHECKER_HPP
#define WIN_CONDITION_CHECKER_HPP

#include <memory>
#include <string>
#include <vector>

class Player;
class GameBoard;

enum class WinMode {
    NONE,
    BANKRUPTCY,
    MAX_TURN
};

class WinConditionChecker {
private:
    int maxTurn;

    int countActivePlayers(GameBoard* board) const;
    int countOwnedProperties(const std::string& username, GameBoard* board) const;

public:
    WinConditionChecker();
    ~WinConditionChecker() = default;

    void setMaxTurn(int turns);
    int  getMaxTurn() const;
    bool isMaxTurnValid() const;

    bool isBankruptcyEnd(GameBoard* board) const;
    bool isMaxTurnEnd(GameBoard* board) const;
    bool isGameOver(GameBoard* board) const;

    WinMode getEndMode(GameBoard* board) const;

    std::vector<std::shared_ptr<Player>> determineWinners(GameBoard* board) const;
};

#endif
