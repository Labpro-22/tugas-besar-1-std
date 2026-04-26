#ifndef SKILLCARD_H
#define SKILLCARD_H

#include <string>

// Forward declarations
class Player;
class GameContext;

class SkillCard {
protected:
    std::string cardType;
    int value;              
    int remainingDuration;  

public:
    SkillCard(const std::string& type, int val, int duration);
    virtual ~SkillCard() {}

    // Aktifkan efek kartu
    virtual void activate(Player* player, GameContext* ctx) = 0;

    // Deskripsi singkat untuk display 
    virtual std::string getDescription() const = 0;

    // Kurangi sisa durasi 
    void decrementDuration();

    std::string getCardType() const         { return cardType; }
    int         getValue() const            { return value; }
    int         getRemainingDuration() const { return remainingDuration; }
    void        setRemainingDuration(int d) { remainingDuration = d; }
};


// MoveCard: maju sejumlah langkah acak (ditentukan saat kartu dibuat)

class MoveCard : public SkillCard {
public:
    explicit MoveCard(int steps);
    void activate(Player* player, GameContext* ctx) override;
    std::string getDescription() const override;
};

// DiscountCard: diskon acak (0 - 100%) properti pada giliran ini
class DiscountCard : public SkillCard {
public:
    explicit DiscountCard(int percent);
    void activate(Player* player, GameContext* ctx) override;
    std::string getDescription() const override;
};

// ShieldCard: kebal satu kali tagihan/sanksi selama giliran
class ShieldCard : public SkillCard {
public:
    ShieldCard();
    void activate(Player* player, GameContext* ctx) override;
    std::string getDescription() const override;
};

// TeleportCard: pindah kemanapun di papan
class TeleportCard : public SkillCard {
public:
    TeleportCard();
    void activate(Player* player, GameContext* ctx) override;
    std::string getDescription() const override;
};

// LassoCard: tarik satu pemain lawan di depan ke posisi kita
class LassoCard : public SkillCard {
public:
    LassoCard();
    void activate(Player* player, GameContext* ctx) override;
    std::string getDescription() const override;
};

// DemolitionCard: hancurkan satu bangunan milik lawan
class DemolitionCard : public SkillCard {
public:
    DemolitionCard();
    void activate(Player* player, GameContext* ctx) override;
    std::string getDescription() const override;
};

#endif
