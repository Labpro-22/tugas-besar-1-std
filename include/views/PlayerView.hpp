#ifndef PLAYER_VIEW_HPP
#define PLAYER_VIEW_HPP

#include "../../include/models/Player.hpp"
#include "../../include/models/Tile.hpp"
#include "../../include/models/Property.hpp"
#include "../../include/models/Street.hpp"
#include <vector>
#include <memory>
#include <iomanip>
class Player;

class PlayerView {
public:
    void showPlayerStatus(Player* player, const vector<unique_ptr<Tile>>& tiles);
};

#endif