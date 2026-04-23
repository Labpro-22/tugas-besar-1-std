#include "../include/models/GoTile.hpp"
#include "../include/models/Player.hpp"

GoTile::GoTile(int position, const std::string& name, const std::string& code, const std::string& color, int salary)
    : SpecialTile(position, name, "GO", "DEFAULT", SpecialType::GO), salary(salary) {
}

int GoTile::getSalary() const {
    return salary;
}

void GoTile::setSalary(int amount) {
    if (amount >= 0) {
        salary = amount;
    }
}

void GoTile::giveSalary(Player* player) {
    if (player != nullptr) {
        player->operator+=(salary);
    }
}

void GoTile::onPass(Player* player) {
    if (player != nullptr) {
        giveSalary(player);
    }
}

void GoTile::executeSpecial(Player* player) {
    (void)player;
}
