#include "../../include/views/PlayerView.hpp"

#include <iostream>
#include <iomanip>

using namespace std;

void PlayerView::showPlayerStatus(Player* player, const vector<unique_ptr<Tile>>& tiles) {

    if (!player) {
        cout << "Player tidak ditemukan.\n";
        return;
    }

    string code;
    cout << "Masukkan kode petak: ";
    cin >> code;

    Property* target = nullptr;

    for (const auto& t : tiles) {
        if (t && t->getCode() == code) {
            target = (Property*)t.get();
            break;
        }
    }

    if (!target) {
        cout << "Petak \"" << code << "\" tidak ditemukan atau bukan properti.\n";
        return;
    }

    string category = target->getColor();
    string name = target->getName();

    cout << "+================================+\n";
    cout << "|        AKTA KEPEMILIKAN        |\n";

    string title = "|    [" + category + "] " + name + " (" + target->getCode() + ")";
    if (title.length() < 32)
        title += string(32 - title.length(), ' ');
    title += "|";

    cout << title << endl;
    cout << "+================================+\n";

    cout << "| Harga Beli        : M" << setw(10) << left << target->getPurchasePrice() << "|\n";
    cout << "| Nilai Gadai       : M" << setw(10) << left << target->getMortgageValue() << "|\n";

    cout << "+--------------------------------+\n";

    if (Street* street = (Street*)target) {

        cout << "| Sewa (unimproved) : M" << setw(10) << left << street->getRent(0) << "|\n";
        cout << "| Sewa (1 rumah)    : M" << setw(10) << left << street->getRent(1) << "|\n";
        cout << "| Sewa (2 rumah)    : M" << setw(10) << left << street->getRent(2) << "|\n";
        cout << "| Sewa (3 rumah)    : M" << setw(10) << left << street->getRent(3) << "|\n";
        cout << "| Sewa (4 rumah)    : M" << setw(10) << left << street->getRent(4) << "|\n";
        cout << "| Sewa (hotel)      : M" << setw(10) << left << street->getRent(5) << "|\n";

        cout << "+--------------------------------+\n";

        cout << "| Harga Rumah       : M" << setw(10) << left << street->getHousePrice() << "|\n";
        cout << "| Harga Hotel       : M" << setw(10) << left << street->getHotelPrice() << "|\n";
    } 
    else {
        cout << "| Informasi sewa khusus           |\n";
    }

    cout << "+================================+\n";

    string status;
    if (target->getStatus() == StatusType::OWNED)
        status = "OWNED (" + target->getOwner() + ")";
    else if (target->getStatus() == StatusType::MORTGAGED)
        status = "MORTGAGED";
    else
        status = "BANK";

    cout << "| Status : " << setw(22) << left << status << "|\n";
    cout << "+================================+\n";
}