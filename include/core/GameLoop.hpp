#ifndef GAMELOOP_H
#define GAMELOOP_H

class GameContext;
class Dice;

class GameLoop {
public:
    static void playTurn(GameContext* ctx, Dice& dice);
};

#endif
