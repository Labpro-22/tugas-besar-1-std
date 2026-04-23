#ifndef DICE_H
#define DICE_H

class Dice {
private:
    int d1, d2;
    bool isManualMode;

public:
    Dice();

    void rollRandom();
    void setManual(int a, int b);

    int getTotal();
    bool isDouble();


    int getDie1();
    int getDie2();
    bool getManualMode();
};

#endif
