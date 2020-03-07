#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
// To compile need to include -lreadline before -o
#include <readline/readline.h>

//--- Types declared:

// LinkedList of recently input-strings.
struct StrHashElem_s {
  char *str;
  size_t len;
  struct StrHashElem_s* next;
};

// Now we can refer to it as StrHashElem_t!
typedef struct StrHashElem_s StrHashElem_t;


//--- Methods declared:
// hash any str
int hash(char *str, size_t len);
// construct a new node
StrHashElem_t* node_create(char* str, size_t len);
// hash a node
int node_hash(StrHashElem_t* node);
// print a node
void node_print(StrHashElem_t* node);
// release memory associated with a node
void node_free(StrHashElem_t* node);


// int argc = number of arguments that gets passed in
// char *argv[] = array of strings that are passed in
int main(int argc, char *argv[]) {
	// This will be the "start" or "head" of our linked-list.
	StrHashElem_t *history = NULL;

	// Definition of white space and line char array
	// to be used in string tokenizng
	char *whitespace = " \t\f\r\v\n";
	char *line;

	// Go forever, unless there's an error
        while(true) {
	  // Find out what the user wants to do
	  line = readline("Enter 'history' to see your hash history or 'put X' where X is the string you want to add to the history: "); 
	  // Error catching
	  if (line == NULL){
	    printf("Error readline failed. Exiting...\n");
	    exit(1);
	  }

	  // If the user is asking for history
	  else if (strncmp(line, "history", 7) == 0){
	    // If the user does not have any history
	    if (history == NULL){
	      printf("Cannot print a NULL pointer. Add hashes to history first!\n");
	    }
	    else{
	      // Cycle through the history
	      StrHashElem_t *curr = history;
	      while (curr != NULL){
		node_print(curr);
		curr = curr->next;
	      }
	    }
	  }

	  // Putting a string into history or error catching
	  else{
	    char *token = strtok(line, whitespace);
	    // The user did not type valid input
	    if (token == NULL || strcmp(token, "put") != 0){
	      printf("I do not understand. Please try again or do Ctrl-C to quit.\n");
	    }
	    // The user wants to put a string into history
	    else{
	      // Find out what the string is that the user wants to add
	      token = strtok(NULL, whitespace);
	      while (token != NULL){
		// Create a node for this string and add it to the history
		StrHashElem_t *new_node = node_create(token, strlen(token));
		new_node->next = history;
		history = new_node;
		// If they said "put" with a variety of words
		// (separated by whitespace) add all those words to the history
		token = strtok(NULL, whitespace);
	      }
	    }
	  }
	  free(line);
	}

	// Looping over the linked list of history, calling free on each one.
	StrHashElem_t* curr = history;
	while (curr != NULL){
	  StrHashElem_t *temp = curr->next;
	  node_free(curr);
	  curr = temp;
	}
	
	return 0;
}

StrHashElem_t* node_create(char* str, size_t len) {
  // Create a StrHashElem_t using the given input
  // Use a pointer so this StrHashElem_t will survive
  // after the function returns and we can still use it
  StrHashElem_t *toReturn = malloc(sizeof(StrHashElem_t));
  assert(toReturn != NULL);
  toReturn->str = strdup(str);
  toReturn->len = len;
  toReturn->next = NULL;
  return toReturn;
}

int node_hash(StrHashElem_t* node) {
  // Calculate the hash of the node
  // based on its string contents and length
  return hash(node->str, node->len);
}

void node_print(StrHashElem_t* node) {
  // Print the string the node contains and its hash
  printf("%s:%d\n", node->str, node_hash(node));
}

void node_free(StrHashElem_t* node) {
  assert(node != NULL);
  // First, we free the string inside if we have one.
  if (node->str != NULL) {
    free(node->str);
  }
  // Then, we free the node itself.
  free(node);
}

int hash(char *str, size_t len) {
  assert(len >= 0);
  assert(str != NULL);

  // Using Java's algorithm:
  int hash = 7;
	
  // Mix in every character with primes.
  for (size_t i=0; i<len; i++) {
    hash = (hash * 31) + str[i];
  }

  return hash;
}
