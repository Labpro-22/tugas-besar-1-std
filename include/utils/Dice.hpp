#ifndef DICE_H
#define DICE_H

#include <cstdlib>
#include <ctime>

class Dice {
private:
    int d1, d2;
    bool isManualMode;

public:
    Dice();

    void rollRandom();
    void setManual(int a, int b);

    int getTotal() const;
    bool isDouble() const;
    
    int getDie1() const;
    int getDie2() const;
    bool getManualMode() const;
};

#endif
