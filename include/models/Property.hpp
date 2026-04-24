#pragma once

#include "Tile.hpp"


enum StatusType {
    BANK,
    OWNED,
    MORTGAGED
};

class Property : public Tile {
protected:
    std::string owner;
    StatusType status;
    int festivalMultiplier;
    int festivalDuration;
    int purchasePrice;
    int mortgageValue;

public:
    Property(int position, const std::string& name, const std::string& code, const std::string& color,
             int purchasePrice, int mortgageValue);
    virtual ~Property() = default;

    std::string getOwner() const;
    StatusType getStatus() const;
    int getFestivalMultiplier() const;
    int getFestivalDuration() const;
    int getPurchasePrice() const;
    int getMortgageValue() const;

    void setOwner(const std::string& username);
    void setStatus(StatusType status);
    void setFestivalMultiplier(int multiplier);
    void setFestivalDuration(int duration);

    virtual int calculateRent(Player* visitor) = 0;
    virtual void mortgage();
    virtual void redeem();


    void onLand(Player* player, GameContext* ctx = nullptr) override;
    void onPass(Player* player) override;
};
