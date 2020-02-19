#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

// This code will take a line a user inputs
// and return it in all caps
int main(void) {
	char line[80];
	while(true) {
		// Read a line of input:
		if (fgets(line, 80, stdin) == NULL) {
			break;
		}
		// Capitalize it and print it.
		for (int c = 0; c < 80; c++){
		  line[c] = toupper(line[c]);
		}
		printf("%.80s", line);
	}
   
	return 0;
}
