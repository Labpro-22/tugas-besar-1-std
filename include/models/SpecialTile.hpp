#pragma once

#include "ActionTile.hpp"


enum SpecialType {
    GO,
    JAIL,
    FREE_PARKING,
    GO_TO_JAIL
};

class SpecialTile : public ActionTile {
protected:
    SpecialType specialType;

public:
    SpecialTile(int position, const std::string& name, const std::string& code, const std::string& color,
                SpecialType specialType);
    virtual ~SpecialTile() = default;

    SpecialType getSpecialType() const;
    void setSpecialType(SpecialType type);

    virtual void executeSpecial(Player* player, GameContext* ctx = nullptr) = 0;

    void executeAction(Player* player, GameContext* ctx = nullptr) override;
    virtual void onPass(Player* player) override;  


};
