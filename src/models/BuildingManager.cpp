#include "../include/models/BuildingManager.hpp"
#include "../include/models/Street.hpp"
#include "../include/models/Player.hpp"

bool BuildingManager::canBuildHouse(Street* street, Player* player) {
    if (!street || !player) return false;

    // harus owner
    if (street->getOwner() != player->getUsername()) return false;

    // maksimal 4 rumah
    if (stoi(street->getBuildingCount()) >= 4) return false;

    // uang cukup
    if (player->getMoney() < street->getHousePrice()) return false;

    return true;
}

bool BuildingManager::canBuildHotel(Street* street, Player* player) {
    if (!street || !player) return false;

    if (street->getOwner() != player->getUsername()) return false;

    // harus 4 rumah dulu
    if (stoi(street->getBuildingCount()) != 4) return false;

    if (player->getMoney() < street->getHotelPrice()) return false;

    return true;
}

bool BuildingManager::buildHouse(Street* street, Player* player) {
    if (!canBuildHouse(street, player)) return false;

    player->payVoluntary(street->getHousePrice());
    street->buildHouse();

    return true;
}

bool BuildingManager::buildHotel(Street* street, Player* player) {
    if (!canBuildHotel(street, player)) return false;

    player->payVoluntary(street->getHotelPrice());
    street->buildHotel();

    return true;
}

bool BuildingManager::sellHouse(Street* street, Player* player) {
    if (!street || !player) return false;

    if (street->getOwner() != player->getUsername()) return false;
    if (stoi(street->getBuildingCount()) <= 0) return false;

    street->setBuildingCount(std::to_string(stoi(street->getBuildingCount()) - 1));

    (*player) += street->getHousePrice() / 2;
    return true;
}

bool BuildingManager::sellHotel(Street* street, Player* player) {
    if (!street || !player) return false;

    if (street->getOwner() != player->getUsername()) return false;
    if (stoi(street->getBuildingCount()) != 5) return false;

    street->setBuildingCount(std::to_string(4)); // kembali ke 4 rumah
    (*player) += street->getHotelPrice() / 2;

    return true;
}

bool BuildingManager::checkEvenDistribution(const std::vector<Street*>& streets) {
    if (streets.empty()) return true;

    int minVal = stoi(streets[0]->getBuildingCount());
    int maxVal = stoi(streets[0]->getBuildingCount());

    for (auto* s : streets) {
        int count = stoi(s->getBuildingCount());
        if (count < minVal) minVal = count;
        if (count > maxVal) maxVal = count;
    }

    return (maxVal - minVal) <= 1;
}

int BuildingManager::getMinHouseCount(const std::vector<Street*>& streets) {
    if (streets.empty()) return 0;

    int minVal = stoi(streets[0]->getBuildingCount());

    for (auto* s : streets) {
        if (stoi(s->getBuildingCount()) < minVal) {
            minVal = stoi(s->getBuildingCount());
        }
    }

    return minVal;
}