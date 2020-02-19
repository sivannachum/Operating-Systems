#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include<time.h>
#include <stdlib.h>

// This code creates a guessing game
// A random number is generated and the user tries to guess what it is
// If they guess wrong, hints are given
int main(int argc, char **argv) {
  // Generate the random secret number
  srand(time(NULL));
  int secret = rand()%100;

  puts("Welcome to guessing game!");

  while(true){
    // Print a prompt (part of a line)
    printf("Type your guess and then ENTER: ");
    /* Make sure it becomes visible... otherwise C is allowed
       to wait until you print a new line, e.g. '\n'
       before doing anything.
    */
    fflush(stdout);

    // Create and fill a guess variable.
    int guess = -1;
    // scanf returns number of patterns that matched correctly
    int status = scanf("%d", &guess);
    if (status != 1){
      break;
    }

    // Tell the user what they guessed and some information about it
    // (Correct, too low, or too high)
    printf("You guessed %d", guess);

    if (secret == guess){
      puts("\nCongratulations! You guessed the secret.");
      break;
    }
    else if (secret < guess){
      puts("\nToo high! Guess again.");
    }
    else{
      puts("\nToo low! Guess again.");
    }// ends the if's
  }// ends the while
}// ends main
