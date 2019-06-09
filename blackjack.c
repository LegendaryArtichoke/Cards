#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cards.h"


void screen_intro();
void read_line(char **string);
int hand_value(Card *hand, int nmb_elements, int *value);
void deal_hand(Deck *deck);

int main(){
  // Creating a deck made out of 6 indivudal decks, 0 jokers and 1 shuffle marker. Autoshuffle toggled on.
  Deck *deck = deck_create(1,0,1,1);

  screen_intro(); // Prints out a program introduction on screen.
  /*
    Keeps dealing a hand until the player exits the program with the EXIT command.
    The user input is taken inside the deal_hand function, so if the EXIT command
    is dected, the program will exit from there with an exit(EXIT_SUCCESSFUL) instruction.
  */
  while(1){
    deal_hand(deck); // Deals a hand.
  }
}


// Deals a hand from the deck passed as a parameter.
void deal_hand(Deck *deck){
  printf("\n\n\nDealing hand... good luck!\n\n");

  // Declaring dealer's and player's card variables, the dealer's card that will be shown the player is dealer_faceup.
  Card dealer_faceup, dealer_facedown;
  Card player_card_one, player_card_two;

  int shuffle_drawn = 0; /*
                            As my intent for this program is to shuffle the deck at the end of the current hand if
                            a shuffle marker is drawn, I will use a shuffle_drawn variable to keep track of any drawn
                            shuffle marker. If a shuffle marker will be drawn, the shuffle_drawn variable will be set to 1
                            and at the end of the hand the deck will be shuffled.
                          */

  // Dealing the cards to the dealer and the player, considering the case in which a shuffle marker is drawn
  dealer_faceup = deck_draw(deck);
  while(dealer_faceup.suit == SHUFFLE_MARKER){
    dealer_faceup = deck_draw(deck);
    shuffle_drawn = 1;
  }
  dealer_facedown = deck_draw(deck);
  while(dealer_facedown.suit == SHUFFLE_MARKER){
    dealer_facedown = deck_draw(deck);
    shuffle_drawn = 1;
  }
  player_card_one = deck_draw(deck);
  while(player_card_one.suit == SHUFFLE_MARKER){
    player_card_one = deck_draw(deck);
    shuffle_drawn = 1;
  }
  player_card_two = deck_draw(deck);
  while(player_card_two.suit == SHUFFLE_MARKER){
    player_card_two = deck_draw(deck);
    shuffle_drawn = 1;
  }

  /*
     Storing both player's and dealer's hand in a dynamically allocated Card array. If at any point during the current hand
     the player or the dealer will decide to HIT, the newly dealt cards will be added to this array.
  */
  int player_hand_counter = 0;
  int player_hand_size = 6;
  Card *player_hand = malloc(sizeof(Card) * player_hand_size);
  if(player_hand == NULL){
    printf("ERROR: heap memory full.\n");
    exit(EXIT_FAILURE);
  }
  // Now adding the starting player's hand.
  player_hand[player_hand_counter++] = player_card_one;
  player_hand[player_hand_counter++] = player_card_two;

  int dealer_hand_counter = 0;
  int dealer_hand_size = 6;
  Card *dealer_hand = malloc(sizeof(Card) * dealer_hand_size);
  if(dealer_hand == NULL){
    printf("ERROR: heap memory full.\n");
    exit(EXIT_FAILURE);
  }
  // Now adding the starting dealer's hand.
  dealer_hand[dealer_hand_counter++] = dealer_faceup;
  dealer_hand[dealer_hand_counter++] = dealer_facedown;

  // Printing on screen the drawn cards
  printf("Dealer's face up card:\n%s of %s\n\n", value(dealer_faceup), suit(dealer_faceup));
  printf("Your hand:\n%s of %s\t%s of %s\n", value(player_card_one), suit(player_card_one), value(player_card_two), suit(player_card_two));

  // Checks if the player has made blackjack and exits the function if he did.
  if((player_card_one.value == ACE || player_card_two.value == ACE) && (is_figure(player_card_one) || is_figure(player_card_two))){
    printf("\nBLACKJACK!\n");

    // Freeing both player's and dealer's hand to avoid memory leaks.
    free(player_hand);
    free(dealer_hand);
    // Shuffling the deck if a shuffle marker has previouslt been drawn.
    if(shuffle_drawn){
      deck_shuffle(deck);
    }

    /* The program waits for the user to press enter before continuing. This is done to increase enjoyability
       of the game, as if the whole dealt hand was printed out all at once in matters of milliseconds, the game
       would get boring real quick. Waiting for user input increase engagement and slows the pace of the match down.
    */
    printf("PRESS ENTER TO CONTINUE");
    getchar();

    return;
  }

  int player_hand_value = 0; // Stores player's hand value.
  // Printing out player's hand value. For detailed infos on how the hand_value function works check out the function definition.
  hand_value(player_hand, player_hand_counter, &player_hand_value);
  printf("Your hand value: %d\n", player_hand_value);

  /*
     The following while loop is the player's turn. Player's input is asked until his turn is over. To determine
     weather his turn is over or not, the player_turnover variable is used, which will be set to 1 once the player
     will do his last move.
  */
  int player_turnover = 0;
  do{
    printf("\nWhat's your move? ");
    char *input; // Stores the player's move.
    read_line(&input); // Gets player's move.

    if(strcmp(input, "STAND") == 0){ // The player stands.
      /*
         Getting the player's hand value with the hand_value function. hand_value returns either 1 if the
         player busted or hit 21 or 0 if he didn't. The returned number is usually assigned to player_turnover
         to determine weather the player busted or not and if his turn it's over. Here though, the player
         stood so the returned value is ignored and the player_turnover variable is manually assigned to 1.
      */
      (void) hand_value(player_hand, player_hand_counter, &player_hand_value);
      player_turnover = 1; // The player's turn is over.
    }

    else if(strcmp(input, "HIT") == 0){ // The player hits.
      // Dealing a new card and considering the case in which a shuffle marker is drawn.
      Card new_card = deck_draw(deck);
      while(new_card.suit == SHUFFLE_MARKER){
        new_card = deck_draw(deck);
        shuffle_drawn = 1;
      }
      /*
         The newly delt card is now added to the player_hand array. I'm checking if the array is full beforehand
         and expanding its allocated memory if it is.
      */
      if(player_hand_counter == player_hand_size){ // player_hand array is full.
        player_hand_size *= 2;
        void *temp = realloc(player_hand, player_hand_size * sizeof(Card));
        if(temp){
          player_hand = temp;
        }
        else{
          printf("ERROR: heap memory full.\n");
          exit(EXIT_FAILURE);
        }
      }
      player_hand[player_hand_counter++] = new_card; // Adds the newly dealt card to the player_hand array.

      /*
         I'm now getting the numerical player's hand value. Eg.: if the player has a Queen and a Seven his hand value will be 17.
         This is done by using the hand_value() function, which returns 1 if the player either busts or hits 21 (no more moves
         to be made). The returned value is assigned to the player_turnover variable.
      */
      player_turnover = hand_value(player_hand, player_hand_counter, &player_hand_value);
      // Printing on screen the newly dealt card and the player's hand value.
      printf("You've drawn a %s of %s. Your hand's value: %d.\n", value(new_card), suit(new_card), player_hand_value);
    }

    else if(strcmp(input, "EXIT") == 0){ // The player wants to exit the program.
      printf("\n\nThanks for playing!\n\n");
      exit(EXIT_SUCCESS);
    }
    else{ // The player has entered an invalid command.
      printf("Unknown command. Please try again!\n");
      printf("Available commands: HIT, STAND, EXIT.\n");
    }
    // Frees the memory allocated in the read_line function to input, so that no memory leak will occur upon new read_line call.
    free(input);
  }while(player_turnover == 0);

  /*
     At this point, the player's turn is over. He either busted or hit blackjack.
     What I'm doing now is playing the dealer's turn only if the player hasn't busted or hit blackjack as in those cases
     the match would be over anyways. To do that, I'm simply checking if the player_hand_value variable is greater of or
     equal to 21.
  */
  if(player_hand_value == 21){
    printf("BLACKJACK!\n");

    // Freeing both player's and dealer's hand to avoid memory leaks.
    free(player_hand);
    free(dealer_hand);
    // Shuffling the deck if a shuffle marker has previously been drawn.
    if(shuffle_drawn){
      deck_shuffle(deck);
    }
    /* The program waits for the user to press enter before continuing. This is done to increase enjoyability
       of the game, as if the whole dealt hand was printed out all at once in matters of milliseconds, the game
       would get boring real quick. Waiting for user input increase engagement and slows the pace of the match down.
    */
    printf("PRESS ENTER TO CONTINUE");
    getchar();

    return;
  }
  else if(player_hand_value > 21){
    printf("\nYOU BUSTED.\n\n");
    // Freeing both player's and dealer's hand to avoid memory leaks.
    free(player_hand);
    free(dealer_hand);
    // Shuffling the deck if a shuffle marker has previously been drawn.
    if(shuffle_drawn){
      deck_shuffle(deck);
    }
    /* The program waits for the user to press enter before continuing. This is done to increase enjoyability
       of the game, as if the whole dealt hand was printed out all at once in matters of milliseconds, the game
       would get boring real quick. Waiting for user input increase engagement and slows the pace of the match down.
    */
    printf("PRESS ENTER TO CONTINUE");
    getchar();

    return;
  }


  // If the code reaches this point, the player hasn't busted or hit blackjack so the dealer needs to play his turn.
  int dealer_turnover = 0; /* This variable is assigned to 1 once the dealer's turn is over. This happens either when
                              the dealer busts or hits 17 and higher
                           */
  int dealer_hand_value = 0; // Stores dealer's hand value
  do {
    dealer_turnover = hand_value(dealer_hand, dealer_hand_counter, &dealer_hand_value); /*
                                                                                           Similarly to the player's turn while
                                                                                           loop, the dealer's hand value is calculated.
                                                                                           Remember that hand_value will return 1 if
                                                                                           dealer_hand is greater of or equal to 21.
                                                                                        */
    // Before doing anything, the dealer's hand is printed on screen, together with his hand value.
    printf("\nDealer's hand:\n");
    // Prints out all the dealer's cards
    for(int i = 0; i < dealer_hand_counter; i++){
      printf("%s of %s\n", value(dealer_hand[i]), suit(dealer_hand[i]));
    }
    printf("Dealer's hand value: %d\n\n", dealer_hand_value);

    /* The program waits for the user to press enter before continuing. This is done to increase enjoyability
       of the game, as if the whole dealt hand was printed out all at once in matters of milliseconds, the game
       would get boring real quick. Waiting for user input increase engagement and slows the pace of the match down.
    */
    printf("PRESS ENTER TO CONTINUE");
    getchar();

    if(dealer_hand_value >= 17 && dealer_hand_value <= 21){ // If this condition is verified, the dealer stands and his turn is over.
      printf("\nDealer stands.\n\n");
      dealer_turnover = 1;
      /* The program waits for the user to press enter before continuing. This is done to increase enjoyability
         of the game, as if the whole dealt hand was printed out all at once in matters of milliseconds, the game
         would get boring real quick. Waiting for user input increase engagement and slows the pace of the match down.
      */
      printf("PRESS ENTER TO CONTINUE");
      getchar();
    }
    else if(dealer_hand_value < 17){ // Dealer has to hit no matter what if his hand value is less than 17
      printf("\nDealer hits.\n\n");
      // Dealing a new card and considering the case in which a shuffle marker is drawn.
      Card new_card = deck_draw(deck);
      while(new_card.suit == SHUFFLE_MARKER){
        new_card = deck_draw(deck);
        shuffle_drawn = 1;
      }
      // Before adding the newly dealt card to the dealer_hand array, I'm checking if its full and expanding it if it is.
      if(dealer_hand_counter == dealer_hand_size){
        dealer_hand_size *= 2;
        void *temp = realloc(dealer_hand, dealer_hand_size);
        if(temp){
          dealer_hand = temp;
        }
        else{
          printf("ERROR: heap memory full.\n");
          exit(EXIT_FAILURE);
        }
      }
      dealer_hand[dealer_hand_counter++] = new_card; // Adding the newly dealt card to the dealer's hand.
      /* The program waits for the user to press enter before continuing. This is done to increase enjoyability
         of the game, as if the whole dealt hand was printed out all at once in matters of milliseconds, the game
         would get boring real quick. Waiting for user input increase engagement and slows the pace of the match down.
      */
      printf("PRESS ENTER TO CONTINUE");
      getchar();
    }
    else{ // If the dealer's hand value isn't >= than 17, <= 21 or < 17, he must have busted, so the player wins and the match is over.
      printf("\nDealer busts.\nYou won!\n\n");
      // As the dealer has busted, the match is over and the function can be exited.
      // Freeing both player's and dealer's hand to avoid memory leaks.
      free(player_hand);
      free(dealer_hand);
      // Shuffling the deck if a shuffle marker has previouslt been drawn.
      if(shuffle_drawn){
        deck_shuffle(deck);
      }
      /* The program waits for the user to press enter before continuing. This is done to increase enjoyability
         of the game, as if the whole dealt hand was printed out all at once in matters of milliseconds, the game
         would get boring real quick. Waiting for user input increase engagement and slows the pace of the match down.
      */
      printf("PRESS ENTER TO CONTINUE");
      getchar();

      return;
    }
  } while(dealer_turnover == 0);

  // Dealer's turn is over. Determining weather player or dealer won the match.
  printf("\nDealer's hand: %d\nPlayer's hand: %d\n", dealer_hand_value, player_hand_value);
  if(dealer_hand_value > player_hand_value){
    printf("Dealer wins.\n\n");
  }
  else if(dealer_hand_value < player_hand_value){
    printf("Player wins.\n\n");
  }
  else{
    printf("Tie.\n\n");
  }

  // Freeing both player's and dealer's hand to avoid memory leaks.
  free(player_hand);
  free(dealer_hand);

  // Shuffling the deck if a shuffle marker has previouslt been drawn.
  if(shuffle_drawn){
    deck_shuffle(deck);
  }

  /* The program waits for the user to press enter before continuing. This is done to increase enjoyability
     of the game, as if the whole dealt hand was printed out all at once in matters of milliseconds, the game
     would get boring real quick. Waiting for user input increase engagement and slows the pace of the match down.
  */
  printf("PRESS ENTER TO CONTINUE");
  getchar();
}

/*
   The hand_value function is used to get the hand passed as a parameter numerical value, which will get assigned to the value
   pointer passed as a parameter. Eg.: if hand contains a Queen and a Seven, value will be equal to 17.
   A 1 is returned if the player busts or hits 21, else 0 is returned.
*/
int hand_value(Card *hand, int nmb_elements, int *value){
  /*
     To get the hand value, a simple switch is used, summing the value of each card
     in the hand array to a sum variable.
     As aces can be counted both as 11s and as 1s, to get the correct value of the ace
     the following method is adopted: all aces are initially counted as 11s. If after
     this calculation the hand value is equal or lower than 21, the correct value has
     been assigned. If the hand value is greater than 21, the hand_value function gets
     called recursively, counting the first ace in the hand as a 1 instead of an 11. If there
     is more than one ace in the hand, only the first one is counted as a 1 initially, and
     if the hand value turns out to be greater than 21 again, the function keeps getting
     called recursively until either the hand value turns out to be smaller than 21 or
     until each ace has been counted as a 1.
     To count the aces as one, their face value is subtitued with a temporary value of 444 (random number that will not
     cause any conflict with any other existing value).
     To avoid corrupting the hand array passed as a parameter, for all the calculations
     a hand_copy array is used.
  */

  int sum = 0; // Hand value.
  int temp_ace_value = 444; // Temporary value assigned to aces that need to be counted as 1s.
  // Declaring and allocating enough memory for the hand copy array
  Card *hand_cpy = malloc(nmb_elements * sizeof(Card));
  if(hand_cpy == NULL){
    printf("ERROR: heap memory full.\n");
    exit(EXIT_FAILURE);
  }

  memcpy(hand_cpy, hand, nmb_elements * sizeof(Card)); // Copying the array passed as a parameter into the hand copy array

  for(int i = 0; i < nmb_elements; i++){
    int temp = hand_cpy[i].value; // Silencing warning "warning: case value not in enumerated type 'Value'"
    switch (temp) {
      case 0: sum += 11; // Ace has a value of 11.
              break;
      case 1: sum += 2;
              break;
      case 2: sum += 3;
              break;
      case 3: sum += 4;
              break;
      case 4: sum += 5;
              break;
      case 5: sum += 6;
              break;
      case 6: sum += 7;
              break;
      case 7: sum += 8;
              break;
      case 8: sum += 9;
              break;
      // Ten, jack, queen and king all have the same value.
      case 9:
      case 10:
      case 11:
      case 12: sum += 10;
               break;
      case 444: sum += 1; // Ace has a value of 1.
                break;
      default: printf("ERROR: unexpected error in the hand_value function.\n");
               break;

    }
  }

  /*
     If the hand value is greater than 21, checks if there are any ace in the hand_cpy array.
     If an ace is found, its face value is switched with an arbitrarily assigned temp value
     and the hand_value function is called recursively. If no ace is found, nothing is done.
  */
  if(sum > 21){
    for(int i = 0; i < nmb_elements; i++){
      if(hand_cpy[i].value == ACE){
        hand_cpy[i].value = temp_ace_value; // Switching the old ace value with a new temp value, so that the ace can be counted as a 1.
        (void) hand_value(hand_cpy, nmb_elements, &sum); // The returned value of hand_value is ignored purposely.
      }
    }
  }

  *value = sum; // Modifying the value of the variable passed as a parameter to the actual hand value.
  free(hand_cpy); /*
                     Freeing hand copy to avoid memory leaks. Note that every mem_cpy variable allocated during recursive
                     function calls will always be freed as, at one point or another, the function will always reach this point.
                  */
  if(sum >= 21){
    return 1;
  }
  return 0;
}


// Prints out a program introduction on screen.
void screen_intro(){
  puts("");
  puts(",-----.  ,--.              ,--.      ,--.              ,--.     ");
  puts("|  |) /_ |  | ,--,--. ,---.|  |,-.   `--' ,--,--. ,---.|  |,-.  ");
  puts("|  .-.  \\|  |' ,-.  || .--'|     /   ,--.' ,-.  || .--'|     /  ");
  puts("|  '--' /|  |\\ '-'  |\\ `--.|  \\  \\   |  |\\ '-'  |\\ `--.|  \\  \\  ");
  puts("`------' `--' `--`--' `---'`--'`--'.-'  / `--`--' `---'`--'`--' ");
  puts("                                   '---'                        ");
  printf("Welcome.\n\t•The game is played with 8 decks.\n\t•Dealer stands on 17.\n\n\t");
  printf("Game controls: HIT, STAND.\n\tEXIT at any time to exit the game.\n");
}

// Simple read_line function, used to get the player's moves.
void read_line(char **string){
  int string_counter = 0;
  int string_size = 10;
  char c;

  *string = malloc(string_size);
  if(*string == NULL){
    printf("ERROR: heap memory full.\n");
    exit(EXIT_FAILURE);
  }

  while((c = getchar()) != '\n'){
    if(string_counter == string_size - 1){
      string_size *= 2;
      void *temp = realloc(*string, string_size);
      if(temp){
        *string = temp;
      }
      else{
        printf("ERROR:heap memory full.\n");
        exit(EXIT_FAILURE);
      }
    }

    (*string)[string_counter++] = c;
  }

  (*string)[string_counter++] = '\0';
  void *temp = realloc(*string, string_counter);
  if(temp){
    *string = temp;
  }
  else{
    printf("ERROR:heap memory full.\n");
    exit(EXIT_FAILURE);
  }
}
