#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cards.h"

void screen_intro();
void read_line(char **string);

int main(){
  // Creating a deck made out of 6 indivudal decks, 0 jokers and 1 shuffle marker. Autoshuffle toggled off.
  // Deck *deck = deck_create(6,0,1,0);
  char *input; // Storage for user input

  screen_intro();
  read_line(&input);
  do {
    puts("Read success!\n");
    free(input);
    read_line(&input);
  } while(strcmp(input, "EXIT") != 0);



}


void screen_intro(){
  puts("");
  puts(",-----.  ,--.              ,--.      ,--.              ,--.     ");
  puts("|  |) /_ |  | ,--,--. ,---.|  |,-.   `--' ,--,--. ,---.|  |,-.  ");
  puts("|  .-.  \\|  |' ,-.  || .--'|     /   ,--.' ,-.  || .--'|     /  ");
  puts("|  '--' /|  |\\ '-'  |\\ `--.|  \\  \\   |  |\\ '-'  |\\ `--.|  \\  \\  ");
  puts("`------' `--' `--`--' `---'`--'`--'.-'  / `--`--' `---'`--'`--' ");
  puts("                                   '---'                        ");
  printf("Welcome.\n\t•The game is played with 8 decks.\n\t•Dealer stands on 17.\n\t•Split with any two starting cards.");
  printf("\n\t•You can only split cards with the same face value.\n\t•You can only split once per hand.");
  printf("\n\t•One card dealt for each split ace.\n\t•You can't double down after splitting.");
  printf("\n\t•Insurance is offered if the dealer shows an ace.\n\t•Blackjack pays 3:2.\n\t•Insurance pays 2:1.\n");
  printf("Game controls: HIT, STAND, SPLIT, DOUBLE DOWN. EXIT at any time to exit the game.\n");
}

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
