#include "../../include/core/GameContext.hpp"
#include "../../include/models/GameBoard.hpp"

int GameContext::getBoardSize() const {
    if (board != nullptr && !board->getTiles().empty()) {
        return static_cast<int>(board->getTiles().size());
    }
    return 40;
}
