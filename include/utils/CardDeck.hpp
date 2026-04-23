#ifndef CARDSDECK_HPP
#define CARDSDECK_HPP

#include <cstdlib>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
class CardDeck {
private:
    std::vector<T> deck;
    std::vector<T> discardPile;

public:
    CardDeck() = default;

    void addToDeck(T card) {
        if (card != nullptr) {
            deck.push_back(card);
        }
    }

    void shuffle() {
        if (deck.size() < 2) {
            return;
        }

        for (size_t i = deck.size() - 1; i > 0; --i) {
            size_t j = static_cast<size_t>(std::rand() % (i + 1));
            std::swap(deck[i], deck[j]);
        }
    }

    T drawTop() {
        if (deck.empty()) {
            reshuffleDiscardIntoDeck();
        }

        if (deck.empty()) {
            return T{};
        }

        T card = deck.back();
        deck.pop_back();
        return card;
    }

    void discard(T card) {
        if (card != nullptr) {
            discardPile.push_back(card);
        }
    }

    void reshuffleDiscardIntoDeck() {
        while (!discardPile.empty()) {
            deck.push_back(discardPile.back());
            discardPile.pop_back();
        }

        shuffle();
    }

    bool isEmpty() const {
        return deck.empty() && discardPile.empty();
    }

    size_t deckSize() const {
        return deck.size();
    }

    size_t discardSize() const {
        return discardPile.size();
    }

    void clear() {
        deck.clear();
        discardPile.clear();
    }

    const std::vector<T>& getDeck() const {
        return deck;
    }

    const std::vector<T>& getDiscardPile() const {
        return discardPile;
    }

    void deleteAllCards() {
        if constexpr (std::is_pointer<T>::value) {
            for (T card : deck) {
                delete card;
            }

            for (T card : discardPile) {
                delete card;
            }
        }

        clear();
    }
};

#endif