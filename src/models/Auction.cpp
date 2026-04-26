#include "../include/models/Auction.hpp"
#include "../include/models/Player.hpp"
#include "../include/models/Property.hpp"

Auction::Auction(std::shared_ptr<Property> prop,
                 const std::vector<std::shared_ptr<Player>>& players)
    : property(prop), currentBid(0), consecutivePassCount(0), active(false) {

    for (auto& p : players) {
        participants.push_back(p);
    }
}

void Auction::startAuction() {
    currentBid = 0;
    currentBidder.reset();
    consecutivePassCount = 0;
    active = true;
}

bool Auction::placeBid(std::shared_ptr<Player> player, int bidAmount) {
    if (!active || !player) return false;

    if (bidAmount <= currentBid) return false;
    if (player->getMoney() < bidAmount) return false;

    currentBid = bidAmount;
    currentBidder = player;
    consecutivePassCount = 0;

    return true;
}

void Auction::pass(std::shared_ptr<Player> player) {
    if (!active || !player) return;
    consecutivePassCount++;
}

bool Auction::isAuctionEnded() const {
    return active && consecutivePassCount >= (int)participants.size() - 1;
}

std::shared_ptr<Player> Auction::getWinner() const {
    return currentBidder.lock();
}