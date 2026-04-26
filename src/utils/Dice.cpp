#include <cstdlib>
#include <ctime>
#include "../../include/utils/Dice.hpp"

Dice::Dice() {
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }

    d1 = d2 = 1;
    isManualMode = false;
}

void Dice::rollRandom() {
    d1 = rand() % 6 + 1;
    d2 = rand() % 6 + 1;
    isManualMode = false;
}

void Dice::setManual(int a, int b) {
    d1 = a;
    d2 = b;
    isManualMode = true;
}

int Dice::getTotal() { return d1 + d2; }
bool Dice::isDouble() { return d1 == d2; }

int Dice::getDie1() { return d1; }
int Dice::getDie2() { return d2; }
bool Dice::getManualMode() { return isManualMode; 
}
