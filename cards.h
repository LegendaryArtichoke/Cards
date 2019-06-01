#ifndef CARDS_H
#define CARDS_H

#define CARDS_PER_SUIT  13
#define SUITS_NUMBER 4

typedef enum {HEARTS, DIAMONDS, SPADES, CLUBS, JOKER, SHUFFLE_MARKER} Suit;
typedef enum {ACE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, JOKER_VALUE, SHUFFLE_MARKER_VALUE} Value;

typedef struct{
  Suit suit;
  Value value; // Face value of the card
} Card;

/*
   The point of the Drawn_cards struct is to keep track of all the cards that have been
   drawn in a specific deck, so that already drawn card aren't drawn again.
   Cards are saved into the cards array in this order: ace to king, hearts, diamonds,
   spades and clubs. If a card is drawn but no more copies of that card should exist in the
   deck, it is simply discarded and a redraw occurs. Keep in mind that the maximum number of
   occurencies that a card can have is (deck.cards_nmb / CARDS_IN_DECK).
*/
typedef struct{
  int cards[SUITS_NUMBER][CARDS_PER_SUIT];
  int jokers;
} Drawn_cards;

typedef struct{
  int cards_nmb;            // Number of cards in the deck
  int jokers;               // Number of jokers in the deck
  int shuffle_markers;      // Number of shuffle markers in the deck.
  int autoshuffle;          // Allows the user to turn the draw autoshuffle feature on or off.
  Drawn_cards checker;
} Deck;


/*
   Creates a new deck given user parameters and returns it.
   deck_nmb refers to the number of unique decks of which the new deck will be made out of.
   For example, usually in blackjack 6 decks are used at once, all shuffled between eachother,
   so deck_nmb would be equal to 6.
   jokers = number of jokers in the deck
   shuffle_markers = Number of shuffle markers in the deck.
   toggle_autoshuffle = allows to toggle on or off the autoshuffle feature, where the deck is automatically
   shuffled when an attempt to draw is made but there are no more cards to draw. 1 = autoshuffle on,
   0 = autoshuffle off.
*/
Deck *deck_create(int deck_nmb, int jokers, int shuffle_markers, int toggle_autoshuffle);

/*
   Draws a card from the deck passed as a parameter and returns it.
   If a draw attempt is made but there are no more cards left to draw, the deck is shuffled
   automatically, unless specified otherwise by the user during the deck creation process.
   For detaield infos on how a random card is drawn, check the cards.c file.
*/
Card deck_draw(Deck *deck);

/*
   Shuffles the deck passed as a parameter. This implies resetting the deck.checker.cards array and the
   deck.checker.jokers variable. "Shuffling", in this case, does NOT mean shuffling all the cards that
   are yet to be drawn in the deck. Think of it as collecting all the cards that have already been dealt
   and shuffling them back with the cards that haven't been drawn.
*/
void deck_shuffle(Deck *deck);

// Trashes a deck out
void deck_free(Deck *deck);

/*
   Removes a selected card from the deck passed as a parameter. Returns 1 if the operation was successfull,
   else 0 is returned.
   To remove the card, the corresponding slot in the deck.checker.cards array is increased by 1, making it
   as if it had already been drawn.
*/
int card_remove(Card card, Deck *deck);

// Returns weather a card passed as a parameter is a figure or not.
int is_figure(Card card);

// Returns the number of cards that have NOT been drawn in the deck
int cards_available(Deck *deck);

/*
   Cheks if all the cards (shuffle markers and jokers not included) in the deck passed as a parameter
   have been drawn. It does so by checking if all the values in the deck.checker.cards array have
   reached the maximum number of occurencies.
*/
int all_cards_drawn(Deck *deck);

// Cheks if all the jokers in the deck passed as a parameter have been drawn.
int all_jokers_drawn(Deck *deck);

// Returns a string literal for the value of the card passed as a parameter.
const char *value(Card card);

// Returns a string literal for the suit of the card passed as a parameter.
const char *suit(Card card);

#endif
