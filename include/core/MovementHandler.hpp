#ifndef MOVEMENTHANDLER_H
#define MOVEMENTHANDLER_H

#pragma once
#include "../models/JailTile.hpp"

class GameBoard;
class Player;
class Dice;
class GameContext;
class JailTile;

class MovementHandler {
private:
    GameBoard* board;
    GameContext* ctx;  // ganti TileContext
    int boardSize;
    int goSalary;
    int jailFine;

public:
    MovementHandler(GameBoard* b, GameContext* ctx,
                    int boardSize = 40, int goSalary = 200, int jailFine = 50);

    void movePlayer(Player* player, int steps);
    void teleportPlayer(Player* player, int target);
    void pullPlayer(Player* target, int toPosition);
    void handlePassGo(Player* player);
    void sendToJail(Player* player);
    JailResult handleJailTurn(Player* player, Dice& dice);
};

#endif
