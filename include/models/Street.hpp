#pragma once

#include "Property.hpp"

#include <map>

enum ColorGroup {
    COKELAT,
    BIRU_MUDA,
    PINK,
    ORANGE,
    MERAH,
    KUNING,
    HIJAU,
    BIRU_TUA
};

class Street : public Property {
private:
    ColorGroup colorGroup;
    std::map<int, int> rentTable;
    int housePrice;
    int hotelPrice;
    std::string buildingCount;

public:
    Street(int position, const std::string& name, const std::string& code, const std::string& color,
           ColorGroup colorGroup, int purchasePrice, int mortgageValue,
           int housePrice, int hotelPrice);
    Street(int position, const std::string& name, const std::string& color, ColorGroup colorGroup, int purchasePrice, int mortgageValue, int housePrice, int hotelPrice);
    ~Street() = default;

    ColorGroup getColorGroup() const;
    int getRent(int houseCount) const;
    int getHousePrice() const;
    int getHotelPrice() const;
    std::string getBuildingCount() const;

    void setRentTable(const std::map<int, int>& table);
    void setBuildingCount(const std::string& count);

    bool canBuildHouse(Player* player) const;
    bool canBuildHotel(Player* player) const;
    void buildHouse();
    void buildHotel();
    void sellBuilding();

    int calculateRent(Player* visitor) override;
    void mortgage() override;
    void redeem() override;
};
