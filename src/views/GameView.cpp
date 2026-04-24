#include "../../include/views/GameView.hpp"
#include <iostream>

using namespace std;

void GameView::showMessage(const string& msg) {
    cout << msg << endl;
}

void GameView::showError(const string& msg) {
    cout << "[ERROR] " << msg << endl;
}

void GameView::showHeader() {
    cout << "========================================\n";
    cout << "       MONOPOLI GAME - OOP Edition\n";
    cout << "========================================\n\n";
}

void GameView::showTurnInfo(int currentTurn, int maxTurn) {
    cout << "TURN " << currentTurn << " / " << maxTurn << endl;
}


void GameView::showDiceSetDetailed(const string& playerName, int d1, int d2, const string& destination) {
    int total = d1 + d2;

    cout << "Dadu diatur secara manual.\n";
    cout << "Hasil: " << d1 << " + " << d2 << " = " << total << endl;
    cout << "Memajukan Bidak " << playerName << " sebanyak " << total << " petak...\n";
    cout << "Bidak mendarat di: " << destination << "." << endl;
}

void GameView::showInvalidDice() {
    cout << "[ERROR] Nilai dadu tidak valid. Gunakan angka 1 - 6." << endl;
}

void GameView::showDiceRolling() {
    cout << "Mengocok dadu..." << endl;
}

void GameView::showDiceResult(int d1, int d2) {
    cout << "Hasil: " << d1 << " + " << d2 << " = " << (d1 + d2) << endl;
}

void GameView::showPlayerMove(const string& playerName, int steps) {
    cout << "Memajukan Bidak " << playerName << " sebanyak " << steps << " petak..." << endl;
}

void GameView::showLanding(const string& tileName) {
    cout << "Bidak mendarat di: " << tileName << "." << endl;
}

void GameView::showLandingProperty(const string& name, const string& code) {
    cout << "Kamu mendarat di " << name << " (" << code << ")!" << endl;
}

void GameView::showPropertySimple(const string& color, const string& name,
                                  const string& code, int price, int rent) {

    cout << "+================================+\n";
    cout << "| [" << color << "] " << name << " (" << code << ")";

    int len = color.length() + name.length() + code.length() + 6;
    if (len < 32) cout << string(32 - len, ' ');
    cout << "|\n";

    cout << "| Harga Beli    : M" << price << string(11 - to_string(price).length(), ' ') << "|\n";
    cout << "| Sewa dasar    : M" << rent << string(11 - to_string(rent).length(), ' ') << "|\n";
    cout << "| ...                            |\n";
    cout << "+================================+\n";
}

void GameView::showPlayerMoney(int money) {
    cout << "Uang kamu saat ini: M" << money << endl;
}

bool GameView::askBuyProperty(int price) {
    cout << "Apakah kamu ingin membeli properti ini seharga M" << price << "? (y/n): ";
    string ans;
    cin >> ans;
    return (ans == "y" || ans == "Y");
}

void GameView::showPurchaseSuccess(const string& name, int money) {
    cout << name << " kini menjadi milikmu!" << endl;
    cout << "Uang tersisa: M" << money << endl;
}

void GameView::showAuctionStart() {
    cout << "Properti ini akan masuk ke sistem lelang..." << endl;
}

void GameView::showAutoOwned(const string& name) {
    cout << "Belum ada yang menginjaknya duluan, " << name << " kini menjadi milikmu!" << endl;
}

void GameView::showRentPayment(const string& propertyName, const string& code, const string& ownerName, int rent, int playerMoneyBefore, int playerMoneyAfter, int ownerMoneyBefore, int ownerMoneyAfter) {
    cout << "Kamu mendarat di " << propertyName << " (" << code << "), milik " << ownerName << "!\n\n";

    cout << "Sewa         : M" << rent << "\n\n";

    cout << "Uang kamu     : M" << playerMoneyBefore << " -> M" << playerMoneyAfter << "\n";

    cout << "Uang " << ownerName << " : M" << ownerMoneyBefore << " -> M" << ownerMoneyAfter << "\n";
}

void GameView::showMortgagedProperty(const string& propertyName,const string& code,const string& ownerName) {
    cout << "Kamu mendarat di " << propertyName << " (" << code << "), milik " << ownerName << ".\n";

    cout << "Properti ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.\n";
}

void GameView::showCannotPayRent(int rent, int playerMoney) {
    cout << "Kamu tidak mampu membayar sewa penuh! (M" << rent << ")\n";
    cout << "Uang kamu saat ini: M" << playerMoney << "\n";
    cout << "// Alur dilanjutkan ke Kebangkrutan\n";
}

void GameView::showMortgageableList(const vector<Property*>& props) {
    cout << "=== Properti yang Dapat Digadaikan ===\n";

    for (size_t i = 0; i < props.size(); i++) {
        cout << i + 1 << ". "
             << props[i]->getName() << " (" << props[i]->getCode() << ") "
             << "[" << props[i]->getColor() << "] "
             << "Nilai Gadai: M" << props[i]->getMortgageValue() << "\n";
    }

    cout << "\nPilih nomor properti (0 untuk batal): ";
}

void GameView::showMortgageSuccess(const string& name, int value, int money) {
    cout << name << " berhasil digadaikan.\n";
    cout << "Kamu menerima M" << value << " dari Bank.\n";
    cout << "Uang kamu sekarang: M" << money << "\n";
    cout << "Catatan: Sewa tidak dapat dipungut dari properti yang digadaikan.\n";
}

void GameView::showNoMortgageable() {
    cout << "Tidak ada properti yang dapat digadaikan saat ini.\n";
}

void GameView::showCannotMortgage(const string& name, const string& color) {
    cout << name << " tidak dapat digadaikan!\n";
    cout << "Masih terdapat bangunan di color group [" << color << "].\n";
    cout << "Bangunan harus dijual terlebih dahulu.\n\n";
}

void GameView::showBuildingList(const vector<Property*>& group) {
    cout << "Daftar bangunan di color group:\n";

    for (size_t i = 0; i < group.size(); i++) {
        cout << i + 1 << ". " << group[i]->getName() << " -> Nilai jual bangunan: M100\n"; // placeholder
    }
}

bool GameView::askSellBuildings(const string& color) {
    cout << "\nJual semua bangunan color group [" << color << "]? (y/n): ";
    string x; cin >> x;
    return x == "y" || x == "Y";
}

void GameView::showSellBuilding(const string& name, int value, int money) {
    cout << "Bangunan " << name << " terjual. Kamu menerima M"
         << value << ".\n";
    cout << "Uang kamu sekarang: M" << money << "\n";
}

bool GameView::askContinueMortgage(const string& name) {
    cout << "\nLanjut menggadaikan " << name << "? (y/n): ";
    string x; cin >> x;
    return x == "y" || x == "Y";
}

void GameView::showMortgagedList(const vector<Property*>& props) {
    cout << "=== Properti yang Sedang Digadaikan ===\n";

    for (size_t i = 0; i < props.size(); i++) {
        int cost = props[i]->getMortgageValue() * 2;

        cout << i + 1 << ". " << props[i]->getName() << " (" << props[i]->getCode() << ")   " << "[" << props[i]->getColor() << "]  [M]  " << "Harga Tebus: M" << cost << "\n";
    }
}

int GameView::askRedeemChoice() {
    cout << "Pilih nomor properti (0 untuk batal): ";
    int x;
    cin >> x;
    return x;
}

void GameView::showRedeemSuccess(const string& name, int cost, int money) {
    cout << "\n" << name << " berhasil ditebus!\n";
    cout << "Kamu membayar M" << cost << " ke Bank.\n";
    cout << "Uang kamu sekarang: M" << money << "\n";
}

void GameView::showRedeemFailure(const string& name, int cost, int money) {
    cout << "\nUang kamu tidak cukup untuk menebus " << name << ".\n";
    cout << "Harga tebus: M" << cost << " | Uang kamu: M" << money << "\n";
}

void GameView::showNoMortgaged() {
    cout << "Tidak ada properti yang sedang digadaikan.\n";
}

void GameView::showBuildableGroups(const vector<vector<Street*>>& groups) {
    cout << "=== Color Group yang Memenuhi Syarat ===\n";

    for (size_t i = 0; i < groups.size(); i++) {
        cout << i + 1 << ". [" << groups[i][0]->getColor() << "]\n";

        for (auto s : groups[i]) {
            cout << "   - " << s->getName() << " (" << s->getCode() << ")"
                 << " : " << s->getBuildingCount()
                 << " (Harga rumah: M" << s->getHousePrice() << ")\n";
        }
    }
}

int GameView::askGroupChoice() {
    cout << "Pilih nomor color group (0 untuk batal): ";
    int x; cin >> x;
    return x;
}

int GameView::askPropertyChoice() {
    cout << "Pilih petak (0 untuk batal): ";
    int x; cin >> x;
    return x;
}

bool GameView::askUpgradeHotel(int cost) {
    cout << "Upgrade ke hotel? Biaya: M" << cost << " (y/n): ";
    char c; cin >> c;
    return c == 'y' || c == 'Y';
}

void GameView::showNoBuildableGroup() {
    cout << "Tidak ada color group yang memenuhi syarat untuk dibangun.\n";
    cout << "Kamu harus memiliki seluruh petak dalam satu color group terlebih dahulu.\n";
}

void GameView::showBuildSuccess(const string& name, int cost, int money) {
    cout << "Kamu membangun 1 rumah di " << name << ". Biaya: M" << cost << "\n";
    cout << "Uang tersisa: M" << money << "\n";
}

void GameView::showUpgradeSuccess(const string& name, int cost, int money) {
    cout << name << " di-upgrade ke Hotel!\n";
    cout << "Uang tersisa: M" << money << "\n";
}

void GameView::showBuildingState(const vector<Street*>& group) {
    for (auto s : group) {
        cout << "- " << s->getName() << " (" << s->getCode() << ") : "
             << s->getBuildingCount() << "\n";
    }
}

void GameView::showAuctionStart(const string& name, const string& code) {
    cout << "Properti " << name << " (" << code << ") akan dilelang!\n\n";
}

void GameView::showAuctionTurn(const string& playerName) {
    cout << "Giliran: " << playerName << "\n";
    cout << "Aksi (PASS / BID <jumlah>)\n";
    cout << "> ";
}

string GameView::askAuctionAction() {
    string line;
    getline(cin, line);
    return line;
}

void GameView::showHighestBid(int amount, const string& playerName) {
    cout << "Penawaran tertinggi: M" << amount
         << " (" << playerName << ")\n\n";
}

void GameView::showAuctionWinner(const string& playerName, int amount) {
    cout << "Lelang selesai!\n";
    cout << "Pemenang: " << playerName << "\n";
    cout << "Harga akhir: M" << amount << "\n\n";
}

void GameView::showAuctionResult(const string& propertyName, const string& winnerName) {
    cout << "Properti " << propertyName << " kini dimiliki "
         << winnerName << ".\n";
}

void GameView::showFestivalStart() {
    cout << "Kamu mendarat di petak Festival!\n\n";
}

void GameView::showOwnedProperties(const vector<Property*>& props) {
    cout << "Daftar properti milikmu:\n";

    for (auto p : props) {
        cout << "- " << p->getCode()
             << " (" << p->getName() << ")\n";
    }

    cout << endl;
}

string GameView::askPropertyCode() {
    cout << "Masukkan kode properti: ";
    string code;
    cin >> code;
    return code;
}

void GameView::showFestivalActivated(int oldRent, int newRent) {
    cout << "\nEfek festival aktif!\n\n";
    cout << "Sewa awal: M" << oldRent << "\n";
    cout << "Sewa sekarang: M" << newRent << "\n";
    cout << "Durasi: 3 giliran\n";
}

void GameView::showFestivalBoosted(int oldRent, int newRent) {
    cout << "\nEfek diperkuat!\n\n";
    cout << "Sewa sebelumnya: M" << oldRent << "\n";
    cout << "Sewa sekarang: M" << newRent << "\n";
    cout << "Durasi di-reset menjadi: 3 giliran\n";
}

void GameView::showFestivalMax(int currentRent) {
    cout << "\nEfek sudah maksimum (harga sewa sudah digandakan tiga kali)\n\n";
    cout << "Durasi di-reset menjadi: 3 giliran\n";
}

void GameView::showFestivalInvalidCode() {
    cout << "-> Kode properti tidak valid!\n";
}

void GameView::showFestivalNotOwned() {
    cout << "-> Properti bukan milikmu!\n";
}

void GameView::showDebt(const string& creditor, int money, int debt) {
    cout << "Kamu tidak dapat membayar " << debt << " kepada " << creditor << "!\n\n";
    cout << "Uang kamu       : M" << money << "\n";
    cout << "Total kewajiban : M" << debt << "\n";
    cout << "Kekurangan      : M" << (debt - money) << "\n\n";
}

void GameView::showLiquidationEstimate(const vector<string>& items, int total) {
    cout << "Estimasi dana maksimum dari likuidasi:\n";

    for (auto& s : items) cout << "  " << s << "\n";

    cout << "  Total potensi        → M" << total << "\n\n";
}

void GameView::showLiquidationPanel(int money, int debt, const vector<string>& sellList, const vector<string>& mortgageList) {
    cout << "=== Panel Likuidasi ===\n";
    cout << "Uang kamu saat ini: M" << money
         << "  |  Kewajiban: M" << debt << "\n\n";

    cout << "[Jual ke Bank]\n";
    for (auto& s : sellList) cout << s << "\n";

    cout << "\n[Gadaikan]\n";
    for (auto& s : mortgageList) cout << s << "\n";

    cout << "\nPilih aksi (0 jika sudah cukup): ";
}

void GameView::showBankrupt(const string& player, const string& creditor) {
    cout << player << " dinyatakan BANGKRUT!\n";
    cout << "Kreditor: " << creditor << "\n\n";
}

void GameView::showBankruptcyBank(int money) {
    cout << "Uang sisa M" << money << " diserahkan ke Bank.\n";
    cout << "Seluruh properti dikembalikan ke status BANK.\n";
    cout << "Bangunan dihancurkan — stok dikembalikan ke Bank.\n\n";
}

void GameView::showDebtPaid(
    int debt,
    const string& creditor,
    int playerBefore, int playerAfter,
    int creditorBefore, int creditorAfter) {
    cout << "Kewajiban M" << debt << " terpenuhi. Membayar ke "
         << creditor << "...\n";

    cout << "Uang kamu : M" << playerBefore
         << " → M" << playerAfter << "\n";

    cout << "Uang " << creditor << ": M" << creditorBefore
         << " → M" << creditorAfter << "\n";
}

void GameView::showAssetTransferToPlayer(const string& creditor,int money, const vector<string>& assets) {
    cout << "Pengalihan aset ke " << creditor << ":\n";
    cout << "  - Uang tunai sisa  : M" << money << "\n";

    for (auto& a : assets) {
        cout << "  - " << a << "\n";
    }

    cout << endl;
}

void GameView::showPlayerEliminated(const string& player, int remaining) {
    cout << player << " telah keluar dari permainan.\n";
    cout << "Permainan berlanjut dengan " << remaining << " pemain tersisa.\n";
}

void GameView::showAuctionBatchStart() {
    cout << "Properti akan dilelang satu per satu:\n";
}

void GameView::showAuctionItem(const string& name, const string& code) {
    cout << "  → Lelang: " << name << " (" << code << ") ...\n";
}

void GameView::showTotalAsset(int totalAsset) {
    cout << "Total aset + uang tunai          : M" << totalAsset << "\n";
}

void GameView::showInsufficientTotal(int debt) {
    cout << "Tidak cukup untuk menutup kewajiban M" << debt << ".\n\n";
}

void GameView::showAuctionNote() {
    cout << "  [Lelang berjalan sesuai mekanisme LELANG]\n\n";
}

void GameView::showChanceTile() {
    cout << "Kamu mendarat di Petak Kesempatan!\n";
}

void GameView::showCommunityChestTile() {
    cout << "Kamu mendarat di Petak Dana Umum!\n";
}

void GameView::showDrawCard() {
    cout << "Mengambil kartu...\n";
}

void GameView::showCardText(const string& text) {
    cout << "Kartu: \"" << text << "\"\n";
}

void GameView::showCardMove(const string& code) {
    cout << "Bidak dipindahkan ke " << code << ".\n";
}

void GameView::showPayToBank(int amount, int remainingMoney) {
    cout << "Kamu membayar M" << amount << " ke Bank. Sisa Uang = M" << remainingMoney << ".\n";
}

void GameView::showCannotPayCard(const string& reason, int amount, int money) {
    cout << "Kamu tidak mampu membayar " << reason << "! (M" << amount << ")\n";
    cout << "Uang kamu saat ini: M" << money << "\n";
}

void GameView::showSkillCards(const vector<string>& cards) {
    cout << "Daftar Kartu Kemampuan Spesial Anda:\n";

    for (size_t i = 0; i < cards.size(); i++) {
        cout << i + 1 << ". " << cards[i] << "\n";
    }

    cout << "0. Batal\n\n";
}

int GameView::askSkillChoice(int maxChoice) {
    cout << "Pilih kartu yang ingin digunakan (0-" << maxChoice << "): ";
    int x;
    cin >> x;
    return x;
}

void GameView::showSkillActivated(const string& name, const string& desc) {
    cout << "\n" << name << " diaktifkan! " << desc << "\n";
}

void GameView::showSkillAlreadyUsed() {
    cout << "Kamu sudah menggunakan kartu kemampuan pada giliran ini! "
         << "Penggunaan kartu dibatasi maksimal 1 kali dalam 1 giliran.\n";
}

void GameView::showSkillAfterDiceError() {
    cout << "Kartu kemampuan hanya bisa digunakan SEBELUM melempar dadu.\n";
}

void GameView::showShieldBlock(int amount, int money) {
    cout << "[SHIELD ACTIVE]: Efek ShieldCard melindungi Anda!\n";
    cout << "Tagihan M" << amount << " dibatalkan. Uang Anda tetap: M" << money << ".\n";
}

void GameView::showNewSkillCard(const string& cardName) {
    cout << "Kamu mendapatkan 1 kartu acak baru!\n";
    cout << "Kartu yang didapat: " << cardName << ".\n";
}

void GameView::showSkillOverflowWarning() {
    cout << "PERINGATAN: Kamu sudah memiliki 3 kartu di tangan (Maksimal 3)! " << "Kamu diwajibkan membuang 1 kartu.\n\n";
}

void GameView::showSkillCardsWithNew(const vector<string>& cards) {
    cout << "Daftar Kartu Kemampuan Anda:\n";

    for (size_t i = 0; i < cards.size(); i++) {
        cout << i + 1 << ". " << cards[i] << "\n";
    }
    cout << endl;
}

int GameView::askDiscardSkill(int maxChoice) {
    cout << "Pilih nomor kartu yang ingin dibuang (1-" << maxChoice << "): ";
    int x;
    cin >> x;
    return x;
}

void GameView::showSkillDiscarded(const string& cardName, int remaining) {
    cout << "\n" << cardName << " telah dibuang. " << "Sekarang kamu memiliki " << remaining << " kartu di tangan.\n";
}