#include "../include/models/Street.hpp"
#include "../include/models/Player.hpp"

Street::Street(int position, const std::string& name, const std::string& code, const std::string& color,
               ColorGroup colorGroup, int purchasePrice, int mortgageValue,
               int housePrice, int hotelPrice)
    : Property(position, name, code, color, purchasePrice, mortgageValue),
      colorGroup(colorGroup),
      housePrice(housePrice),
      hotelPrice(hotelPrice),
      buildingCount("0") {
}

Street::Street(int position, const std::string& name, const std::string& color, ColorGroup colorGroup, int purchasePrice, int mortgageValue, int housePrice, int hotelPrice)
    : Street(position, name, "", color, colorGroup, purchasePrice, mortgageValue, housePrice, hotelPrice) {
}

ColorGroup Street::getColorGroup() const {
    return colorGroup;
}

int Street::getRent(int houseCount) const {
    auto it = rentTable.find(houseCount);
    if (it != rentTable.end()) {
        return it->second;
    }
    return 0;
}

int Street::getHousePrice() const {
    return housePrice;
}

int Street::getHotelPrice() const {
    return hotelPrice;
}

string Street::getBuildingCount() const {
    return buildingCount;
}

void Street::setRentTable(const std::map<int, int>& table) {
    rentTable = table;
}

void Street::setBuildingCount(const std::string& count) {
    buildingCount = count;
}

bool Street::canBuildHouse(Player* player) const {
    if (player == nullptr || status != StatusType::OWNED) {
        return false;
    }
    return player->getMoney() >= housePrice;
}

bool Street::canBuildHotel(Player* player) const {
    if (player == nullptr || status != StatusType::OWNED) {
        return false;
    }
    return player->getMoney() >= hotelPrice;
}

void Street::buildHouse() {
    if (stoi(buildingCount) < 4) buildingCount = std::to_string(stoi(buildingCount) + 1);
}

void Street::buildHotel() {
    if (stoi(buildingCount) == 4) buildingCount = "H";
}


int Street::calculateRent(Player* visitor) {
    if (status != StatusType::OWNED || visitor == nullptr) {
        return 0;
    }

    int houseCount = 0;
    if(houseCount = std::stoi(buildingCount)) {
        houseCount = std::stoi(buildingCount);
    } else if (buildingCount == "H") {
        houseCount = 5; // 5 untuk hotel
    } else {
        houseCount = 0; // default ke 0 kalau parsing gagal
    }
    int baseRent = getRent(houseCount);
    return baseRent * festivalMultiplier;
}

void Street::mortgage() {
    Property::mortgage();
}

void Street::redeem() {
    Property::redeem();
}