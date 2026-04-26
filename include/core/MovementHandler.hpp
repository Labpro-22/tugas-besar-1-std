#ifndef MOVEMENTHANDLER_H
#define MOVEMENTHANDLER_H

#include "JailTile.hpp"   

class GameBoard;
class Player;
class Dice;
class GameContext;
class TransactionLogger;

class MovementHandler {
private:
    GameBoard*   board;
    GameContext* ctx;
    int boardSize;
    int goSalary;
    int jailFine;

public:
    MovementHandler(GameBoard* board, GameContext* ctx,
                    int boardSize = 40, int goSalary = 200, int jailFine = 50);


    // Caller harus increment consecutiveDoubles sebelum memanggil ini jika dadu double.
    void movePlayer(Player* player, int steps);

    void teleportPlayer(Player* player, int target);
    void pullPlayer(Player* target, int toPosition);
    void handlePassGo(Player* player);
    void sendToJail(Player* player);


    // Caller harus roll dadu sebelum memanggil ini, lalu cek return value
    JailResult handleJailTurn(Player* player, Dice& dice);
};

#endif