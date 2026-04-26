#pragma once

#include <string>
#include "GameController.hpp"

class CommandProcessor {
private:
    GameController* gameController;

public:
    CommandProcessor(GameController* gc);
    void processCommand(const std::string& command);
};