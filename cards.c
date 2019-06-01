#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "cards.h"

const int CARDS_IN_DECK = 52;

static void initialize(int array[SUITS_NUMBER][CARDS_PER_SUIT]);
static void set_seed(int *check);

int seed_checker = 1; /*
                         This variable, together with the set_seed function,
                         is needed not to set the srand seed more than once
                      */



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
Deck *deck_create(int deck_nmb, int jokers, int shuffle_markers, int toggle_autoshuffle){
  Deck *new_deck = malloc(sizeof(Deck));
  if(new_deck == NULL){
    printf("ERROR: heap memory full.\n");
    exit(EXIT_FAILURE);
  }

  new_deck->cards_nmb = deck_nmb * CARDS_IN_DECK;
  new_deck->jokers = jokers;
  new_deck->shuffle_markers = shuffle_markers;
  initialize(new_deck->checker.cards); /* Initializes the cards array, setting all values to 0. This
                                         indicates that no card has been drawn yet */
  new_deck->autoshuffle = toggle_autoshuffle;
  return new_deck;
}

Card deck_draw(Deck *deck){
  /*
     If the set_seed function is called for the first time, it sets the srand seed, else it
     does nothing. It checks if it is called for the first time by looking at the seed_checker
     variable, passed along as a parameter. If it is equal to 1, set_seed has been called for the
     first time.
  */
  set_seed(&seed_checker);

  /*
     Before doing anything, the function checks if there are actually any cards to
     draw inside the deck. If all the cards have already been drawn, it checks the deck.autoshuffle toggle.
     If it is off, a card with an invalid suit and face value is returned.
     If it is on, the deck gets shuffled. This is done by simply resetting the deck.checker.cards array to 0, together with
     the deck.checker.jokers variable
  */
  if(all_cards_drawn(deck) && all_jokers_drawn(deck)){
    if(deck->autoshuffle){ // Autoshuffle toggled on.
      printf("All cards have been drawn. Shuffling...\n");
      initialize(deck->checker.cards);
      deck->checker.jokers = 0;
      printf("The deck has been shuffled.\n\n");
    }
    else{ // Autoshuffle toggled off.
      printf("The deck is empty.\n\n");
      Card card = {99, 99};
      return card;
    }
  }

  /*
     total_cards is actually equal to the cards that have NOT been drawn in the deck.
     This is done to give accurate drawing odds, as the number of available cards
     decreases at each draw.
  */
  int total_cards = cards_available(deck);
  Card drawn_card;

  /*
     Draws a random card. The numbers from 0 to (deck.jokers - deck.checker.jokers - 1) will show that
     a joker has been drawn. deck.jokers - deck.checker.jokers indicates the number of the jokers that are
     yet to be drawn.
     The numbers from (deck.jokers - deck.checker.jokers) to (deck.shuffle_markers + (deck.jokers - deck.checker.jokers) - 1)
     show that a shuffle marker has been drawn, and all the other numbers that a regular
     card has been drawn.
  */

  int card = rand() % total_cards;

  if(card < (deck->jokers - deck->checker.jokers)){
    (deck->checker.jokers)++; //increase the number of the drawn jokers
    drawn_card.suit = JOKER;
    drawn_card.value = JOKER_VALUE;
    return drawn_card;
  }
  else if(card >= (deck->jokers - deck->checker.jokers) && card < deck->shuffle_markers + (deck->jokers - deck->checker.jokers)){
    drawn_card.suit = SHUFFLE_MARKER;
    drawn_card.value = SHUFFLE_MARKER_VALUE;
    return drawn_card;
  }
  else{
    /*
       In this case, a valid card has been drawn. The actual drawn number doesn't
       really matter, as to determine the suit and the face value of the drawn card,
       a random number between 0 and the number of the cards that have NOT been drawn, jokers excluded,
       is generated (let's call this number N). After this, the deck.checker.cards array
       is gone through, and the Nth card that is still elegible to be drawn (its occurencies
       are stricly minor than deck.cards_nmb / CARDS_IN_DECK) is returned.
    */
    int n = rand() % (total_cards - (deck->jokers - deck->checker.jokers));
    n -= CARDS_IN_DECK * ((deck->cards_nmb / CARDS_IN_DECK) - 1); /*
                                                                    This solves any issue that might appear
                                                                    in case the deck is made out of more than
                                                                    one deck, possibly generating a number
                                                                    greater than 52 and resulting in no card
                                                                    being returned in the for loops below.
                                                                  */

    for(int i = 0; i < SUITS_NUMBER; i++){
      for(int k = 0; k < CARDS_PER_SUIT; k++){
        if(deck->checker.cards[i][k] < deck->cards_nmb / CARDS_IN_DECK){ // Card elegible to be drawn found
          n--;
          if(n <= 0){
            drawn_card.suit = i;
            drawn_card.value = k;
            (deck->checker.cards[i][k])++; // Counts the cards as drawn

            return drawn_card;
          }
        }
      }
    }
  }

  /*
     The code should never reach this point, as a card should always be returned before this. If someone ever gets this
     error message, please let me know in the bugs section!
  */
  puts("ERROR: UNEXPECTED ERROR DURING THE DRAWING PHASE");
  exit(EXIT_FAILURE);
}

/*
   Shuffles the deck passed as a parameter. This implies resetting the deck.checker.cards array and the
   deck.checker.jokers variable. "Shuffling", in this case, does NOT mean shuffling all the cards that
   are yet to be drawn in the deck. Think of it as collecting all the cards that have already been dealt
   and shuffling them back with the cards that haven't been drawn.
*/
void deck_shuffle(Deck *deck){
  initialize(deck->checker.cards);
  deck->checker.jokers = 0;
}

/*
   Removes a selected card from the deck passed as a parameter. Returns 1 if the operation was successfull,
   else 0 is returned.
   To remove the card, the corresponding slot in the deck.checker.cards array is increased by 1, making it
   as if it had already been drawn.
*/
int card_remove(Card card, Deck *deck){
  if(deck->checker.cards[card.suit][card.value] < deck->cards_nmb / CARDS_IN_DECK){
    (deck->checker.cards[card.suit][card.value])++; // "Removes the card", basically counting it as drawn.
    return 1; // The card was present in the deck and has been removed successfuly
  }
  return 0; /*
               The card has not been removed, as it wasn't in the deck in the first place.
               No action has been performed on the deck.
            */
}

// Returns the number of cards that have NOT been drawn in the deck
int cards_available(Deck *deck){
  int cards_count = 0;
  int maximum_occurencies = deck->cards_nmb / CARDS_IN_DECK;

  cards_count += (deck->jokers - deck->checker.jokers);

  for(int i = 0; i < SUITS_NUMBER; i++){
    for(int k = 0; k < CARDS_PER_SUIT; k++){
      cards_count += (maximum_occurencies - (deck->checker.cards)[i][k]);
    }
  }

  return cards_count;
}

// Cheks if all the jokers in the deck passed as a parameter have been drawn.
int all_jokers_drawn(Deck *deck){
  if(deck->checker.jokers == deck->jokers){
    return 1;
  }
  return 0;
}

/*
   Cheks if all the cards (shuffle markers and jokers not included) in the deck passed as a parameter
   have been drawn. It does so by checking if all the values in the deck.checker.cards array have
   reached the maximum number of occurencies.
*/
int all_cards_drawn(Deck *deck){
  int max_occurencies = deck->cards_nmb / CARDS_IN_DECK; // Maximum number of occurencies that a card can have.

  for(int i = 0; i < SUITS_NUMBER; i++){
    for(int k = 0; k < CARDS_PER_SUIT; k++){
      if((deck->checker.cards)[i][k] < max_occurencies){
        return 0;
      }
    }
  }
  return 1;
}

// Returns a string literal for the suit of the card passed as a parameter.
const char *suit(Card card){
  switch(card.suit){
    case 0: return "Hearts";
    case 1: return "Diamonds";
    case 2: return "Spades";
    case 3: return "Clubs";
    case 4: return "Joker";
    default: printf("Error: the card has an invalid suit\n\n");
             return "ERROR - SUIT";

  }
}

// Returns a string literal for the value of the card passed as a parameter.
const char *value(Card card){
  switch(card.value){
    case 0: return "Ace";
    case 1: return "Two";
    case 2: return "Three";
    case 3: return "Four";
    case 4: return "Five";
    case 5: return "Six";
    case 6: return "Seven";
    case 7: return "Eight";
    case 8: return "Nine";
    case 9: return "Ten";
    case 10: return "Jack";
    case 11: return "Queen";
    case 12: return "King";
    case 13: return "Joker";
    default: printf("Error: the card has an invalid face value\n\n");
             return "ERROR - VALUE";
  }
}

// Trashes a deck out
void deck_free(Deck *deck){
  free(deck);
}

// Initializes all the values of an array to 0
static void initialize(int array[SUITS_NUMBER][CARDS_PER_SUIT]){
  for(int i = 0; i < SUITS_NUMBER; i++){
    for(int k = 0; k < CARDS_PER_SUIT; k++){
      array[i][k] = 0;
    }
  }
}

/*
   If the set_seed function is called for the first time, it sets the srand seed, else it
   does nothing. It checks if it is called for the first time by looking at the seed_checker
   variable, passed along as a parameter. If it is equal to 1, set_seed has been called for the
   first time.
*/
static void set_seed(int *check){
  if(*check == 1){
    *check = 0;
    srand(time(NULL));
  }
}
