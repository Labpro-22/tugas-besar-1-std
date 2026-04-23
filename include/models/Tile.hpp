#pragma once

#include <string>

// Forward declaration
class Player;

class Tile {
protected:
    int position;
    std::string name;
    std::string color;
    std::string code;

public:
    Tile(int position, const std::string& name, const std::string& code, const std::string& color);
    virtual ~Tile() = default;

    int getPosition() const;
    std::string getName() const;
    std::string getColor() const;
    std::string getCode() const;
    void setCode(const std::string& c);

    virtual void onLand(Player* player) = 0;
    virtual void onPass(Player* player) = 0;
};
