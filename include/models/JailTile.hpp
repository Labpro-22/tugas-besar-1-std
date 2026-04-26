#pragma once

#include "SpecialTile.hpp"

enum JailResult {
    ESCAPED_DOUBLE,
    STILL_JAILED,
    FORCED_OUT,
};


class JailTile : public SpecialTile {
private:
    int fineAmount;

public:
    JailTile(int position, const std::string& name, const std::string& code, const std::string& color, int fineAmount);
    ~JailTile() = default;

    int getFineAmount() const;
    void setFineAmount(int amount);

    void sendToJail(Player* player);
    void visitJail(Player* player);
    JailResult attemptEscape(Player* player);
    void payFine(Player* player);

    void executeSpecial(Player* player, GameContext* ctx = nullptr) override;
};
