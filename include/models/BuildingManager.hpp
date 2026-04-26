#pragma once

#include <vector>

class Street;
class Player;

class BuildingManager {
public:
    bool canBuildHouse(Street* street, Player* player);
    bool canBuildHotel(Street* street, Player* player);

    bool buildHouse(Street* street, Player* player);
    bool buildHotel(Street* street, Player* player);

    bool sellHouse(Street* street, Player* player);
    bool sellHotel(Street* street, Player* player);

    bool checkEvenDistribution(const std::vector<Street*>& streets);
    int getMinHouseCount(const std::vector<Street*>& streets);
};

