#pragma once

#include <vector>
#include <memory>

class Player;
class TurnManager;
class GameBoard;

class WinConditionChecker {
private:
    TurnManager* turnManager;
    GameBoard* gameBoard;

public:
    WinConditionChecker(TurnManager* tm, GameBoard* board);

    // true jika game sudah selesai
    bool isGameOver(const std::vector<std::shared_ptr<Player>>& players) const;

    // Kembalikan pemenang (nullptr jika belum selesai)
    Player* determineWinner(const std::vector<std::shared_ptr<Player>>& players) const;
    std::vector<Player*> determineWinners(const std::vector<std::shared_ptr<Player>>& players) const;

private:
    int countActive(const std::vector<std::shared_ptr<Player>>& players) const;
};
