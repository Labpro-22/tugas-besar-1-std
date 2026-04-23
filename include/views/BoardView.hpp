#ifndef BOARD_VIEW_HPP
#define BOARD_VIEW_HPP

#include <vector>
#include <string>
#include <memory>

#include "../../include/models/GameBoard.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/models/Tile.hpp"
#include "../../include/models/Property.hpp"

class GameBoard;
class Player;
class Tile;
class Property;

class BoardView {
public:
    void showBoard(GameBoard& board, const std::vector<std::shared_ptr<Player>>& players);

private:
    std::string getPlayersOnTile(int pos, const std::vector<std::shared_ptr<Player>>& players);
    std::string getColorCode(const std::string& color);
    pair<string, string> formatTile2Line(Tile* tile, const std::vector<std::shared_ptr<Player>>& players);
    void printTop(GameBoard& board, const std::vector<std::shared_ptr<Player>>& players);
    void printBottom(GameBoard& board, const std::vector<std::shared_ptr<Player>>& players);
    void printMiddle(GameBoard& board, const std::vector<std::shared_ptr<Player>>& players);
};

#endif