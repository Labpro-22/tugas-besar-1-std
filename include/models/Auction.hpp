#pragma once

#include <vector>
#include <memory>
#include "Property.hpp"
#include "Player.hpp"  

class Property;
class Player;

class Auction {
private:
    std::weak_ptr<Property> property;
    int currentBid;
    std::weak_ptr<Player> currentBidder;
    int consecutivePassCount;

    std::vector<std::weak_ptr<Player>> participants;
    bool active;

public:
    Auction(std::shared_ptr<Property> property,
            const std::vector<std::shared_ptr<Player>>& players);

    void startAuction();

    bool placeBid(std::shared_ptr<Player> player, int bidAmount);
    void pass(std::shared_ptr<Player> player);
    void endAuction();

    bool isAuctionEnded() const;
    bool isActive() const;
    int getCurrentBid() const;
    int getParticipantCount() const;
    std::shared_ptr<Property> getProperty() const;
    std::shared_ptr<Player> getCurrentBidder() const;
    std::shared_ptr<Player> getWinner() const;
};
