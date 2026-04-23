#pragma once

#include "SpecialTile.hpp"

class GoTile : public SpecialTile {
private:
    int salary;

public:
    GoTile(int position, const std::string& name, const std::string& code, const std::string& color, int salary);
    ~GoTile() = default;

    int getSalary() const;
    void setSalary(int amount);

    void giveSalary(Player* player);
    void onPass(Player* player) override;
    void executeSpecial(Player* player) override;
};
