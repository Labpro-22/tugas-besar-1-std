#ifndef PROPERTY_VIEW_HPP
#define PROPERTY_VIEW_HPP

#include "../../include/models/Property.hpp"
#include "../../include/models/Player.hpp"
#include "../../include/models/Tile.hpp"

class Property;
class Player;


class PropertyView {
public:
    void showPropertyDetail(Property* property);
    void showPlayerProperties(Player* player, const vector<unique_ptr<Tile>>& tiles);
};

#endif