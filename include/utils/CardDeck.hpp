// include/utils/CardDeck.hpp
#ifndef CARDDECK_HPP
#define CARDDECK_HPP

#include <vector>
#include <algorithm>
#include <cstdlib>
using namespace std;

template <typename T>
class CardDeck {
private:
    vector<T*> deck;
    vector<T*> discardPile;

public:
    CardDeck() {}

    ~CardDeck() {
        for (T* card : deck) delete card;
        for (T* card : discardPile) delete card;
    }

    void addCard(T* card) {
        if (card != nullptr) {
            deck.push_back(card);
        }
    }

    bool isEmpty() const {
        return deck.empty();
    }

    T* drawTop() {
        if (isEmpty()) {
            reshuffleDiscardIntoDeck();
        }
        if (isEmpty()) return nullptr;

        T* card = deck.back();
        deck.pop_back();
        return card;
    }

    void discard(T* card) {
        if (card != nullptr) {
            discardPile.push_back(card);
        }
    }

    void reshuffleDiscardIntoDeck() {
        if (discardPile.empty()) return;

        for (T* card : discardPile) {
            deck.push_back(card);
        }
        discardPile.clear();

        // Fisher-Yates shuffle
        for (int i = (int)deck.size() - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            swap(deck[i], deck[j]);
        }
    }

    void shuffle() {
        for (int i = (int)deck.size() - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            swap(deck[i], deck[j]);
        }
    }

    int size() const { return (int)deck.size(); }
    int discardSize() const { return (int)discardPile.size(); }

    // Untuk save/load — backend butuh akses isi deck
    const vector<T*>& getDeck() const { return deck; }
    const vector<T*>& getDiscardPile() const { return discardPile; }
};

#endif