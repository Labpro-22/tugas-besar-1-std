#ifndef TURNMANAGER_H
#define TURNMANAGER_H

#include <vector>
#include <string>
using namespace std;

class Player;
class GameBoard;

class TurnManager {
private:
    vector<Player*> players;   // urutan bermain 
    vector<string>  turnOrder; // username dalam urutan giliran (untuk save/load)
    int currentIndex;
    int currentTurnNumber;
    int maxTurn;

public:
    TurnManager(const vector<Player*>& players, int maxTurn);

    // Kembalikan player aktif saat ini, Skip bankrut
    Player* getCurrentPlayer();

    // Maju ke giliran berikutnya.
    // jangan panggil saat mendapat double, di handle caller
    void advanceToNextPlayer();

    int  getCurrentTurnNumber() const;
    int  getMaxTurn() const;
    bool isMaxTurnReached() const;

    vector<string> getTurnOrder() const;

    // Susun ulang urutan berdasarkan list username (buat load game).
    void setTurnOrder(const vector<string>& order);

    // Reset hasRolled, hasUsedCard, shield, discount di awal giliran.
    void resetTurnFlags(Player* p);

    // Untuk load game,set nomor turn yang tersimpan.
    void setCurrentTurnNumber(int n);

    int getActivePlayerCount() const;

    // sinkron currentPlayerIndex di GameBoard dengan TurnManager karena kesalahan implementasi
    // Dipanggil GameController setelah advanceToNextPlayer() atau setTurnOrder().
    // (dipakai CommandProcessor untuk getCurrentPlayer()).
    void syncWithBoard(GameBoard* board) const;
};

#endif