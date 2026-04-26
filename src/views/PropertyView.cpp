#include "../../include/views/PropertyView.hpp"

#include <iostream>
#include <iomanip>
#include <map>

using namespace std;

void PropertyView::showPropertyDetail(Property* property) {
    if (!property) return;

    cout << "Property: " << property->getName() << endl;
    cout << "Owner   : " << property->getOwner() << endl;
}

// views/PropertyView.cpp — fungsi showPlayerProperties
void PropertyView::showPlayerProperties(Player* player, const vector<unique_ptr<Tile>>& tiles) {
    if (!player) return;

    cout << "=== Properti Milik: " << player->getUsername() << " ===\n\n";

    map<string, vector<Property*>> groups;
    int total = 0;

    for (const auto& t : tiles) {
        // ── GANTI C-cast dengan dynamic_cast ──
        Property* p = dynamic_cast<Property*>(t.get());
        if (!p) continue;  // skip tile bukan Property (GoTile, JailTile, dll)
        // ── SELESAI ──

        if (p->getOwner() != player->getUsername()) continue;

        groups[p->getColor()].push_back(p);
        total += p->getPurchasePrice();
    }

    if (groups.empty()) {
        cout << "Kamu belum memiliki properti apapun.\n";
        return;
    }

    for (auto& [category, props] : groups) {
        cout << "[" << category << "]\n";
        for (Property* p : props) {
            string status;
            if (p->getStatus() == StatusType::MORTGAGED)
                status = "MORTGAGED [M]";
            else
                status = "OWNED";

            cout << left
                 << setw(28) << ("- " + p->getName() + " (" + p->getCode() + ")")
                 << setw(8)  << ("M" + to_string(p->getPurchasePrice()))
                 << status << "\n";
        }
        cout << "\n";
    }

    cout << "Total kekayaan properti: M" << total << "\n";
}