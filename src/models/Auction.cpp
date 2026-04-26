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

void Auction::endAuction() {
    active = false;
}

bool Auction::isAuctionEnded() const {
    if (!active) return false;
    if (participants.empty()) return true;

    // Jika belum ada bid, lelang berakhir saat semua partisipan pass.
    // Jika sudah ada bid, cukup pass beruntun dari semua partisipan lain.
    int threshold = currentBidder.expired()
        ? static_cast<int>(participants.size())
        : static_cast<int>(participants.size()) - 1;
    return consecutivePassCount >= threshold;
}

bool Auction::isActive() const {
    return active;
}

int Auction::getCurrentBid() const {
    return currentBid;
}

int Auction::getParticipantCount() const {
    return static_cast<int>(participants.size());
}

std::shared_ptr<Property> Auction::getProperty() const {
    return property.lock();
}

std::shared_ptr<Player> Auction::getCurrentBidder() const {
    return currentBidder.lock();
}

std::shared_ptr<Player> Auction::getWinner() const {
    return currentBidder.lock();
}