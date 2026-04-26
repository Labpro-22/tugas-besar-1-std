#include "../../include/views/PropertyView.hpp"
#include "../../include/models/Street.hpp"
#include "../../include/models/Railroad.hpp"
#include "../../include/models/Utility.hpp"

#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>

using namespace std;

static string formatMoney(int amount) {
    string s = to_string(amount);
    string out;
    int count = 0;
    for (int i = (int)s.size() - 1; i >= 0; --i) {
        out.insert(out.begin(), s[i]);
        if (++count % 3 == 0 && i > 0) out.insert(out.begin(), '.');
    }
    return "M" + out;
}

static void printRow(const string& label, const string& value) {
    string line = "| " + label;
    if ((int)line.size() < 21) line += string(21 - line.size(), ' ');
    line += ": " + value;
    if ((int)line.size() < 33) line += string(33 - line.size(), ' ');
    line += "|";
    cout << line << "\n";
}

void PropertyView::showPropertyDetail(Property* property) {
    if (!property) return;

    string category = property->getColor();
    string name = property->getName();
    string code = property->getCode();

    cout << "+================================+\n";
    cout << "|        AKTA KEPEMILIKAN        |\n";

    string title = "|    [" + category + "] " + name + " (" + code + ")";
    if ((int)title.size() < 33) title += string(33 - title.size(), ' ');
    title += "|";
    cout << title << "\n";

    cout << "+================================+\n";
    printRow("Harga Beli", formatMoney(property->getPurchasePrice()));
    printRow("Nilai Gadai", formatMoney(property->getMortgageValue()));
    cout << "+--------------------------------+\n";

    if (Street* s = dynamic_cast<Street*>(property)) {
        printRow("Sewa (unimproved)", formatMoney(s->getRent(0)));
        printRow("Sewa (1 rumah)",    formatMoney(s->getRent(1)));
        printRow("Sewa (2 rumah)",    formatMoney(s->getRent(2)));
        printRow("Sewa (3 rumah)",    formatMoney(s->getRent(3)));
        printRow("Sewa (4 rumah)",    formatMoney(s->getRent(4)));
        printRow("Sewa (hotel)",      formatMoney(s->getRent(5)));
        cout << "+--------------------------------+\n";
        printRow("Harga Rumah", formatMoney(s->getHousePrice()));
        printRow("Harga Hotel", formatMoney(s->getHotelPrice()));
    } else if (Railroad* r = dynamic_cast<Railroad*>(property)) {
        for (int i = 1; i <= 4; ++i) {
            printRow("Sewa (" + to_string(i) + " stasiun)", formatMoney(r->getRent(i)));
        }
    } else if (Utility* u = dynamic_cast<Utility*>(property)) {
        printRow("Sewa (1 utility)", to_string(u->getRent(1, 1)) + " x dadu");
        printRow("Sewa (2 utility)", to_string(u->getRent(1, 2)) + " x dadu");
    }

    cout << "+================================+\n";

    string status;
    if (property->getStatus() == StatusType::OWNED)
        status = "OWNED (" + property->getOwner() + ")";
    else if (property->getStatus() == StatusType::MORTGAGED)
        status = "MORTGAGED";
    else
        status = "BANK";
    printRow("Status", status);
    cout << "+================================+\n";
}

void PropertyView::showPlayerProperties(Player* player, const vector<unique_ptr<Tile>>& tiles) {
    if (!player) return;

    cout << "=== Properti Milik: " << player->getUsername() << " ===\n\n";

    map<string, vector<Property*>> groups;
    int total = 0;

    for (const auto& t : tiles) {
        Property* p = dynamic_cast<Property*>(t.get());
        if (!p) continue;
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