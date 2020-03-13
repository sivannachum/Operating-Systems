#include "hashtable.h"
#include "bytes.h"
#include <ctype.h>
#include <stdlib.h>

/* 
 * skip_whitespace
 *
 * Inputs:
 *  - buffer: the string to read from
 *  - start: where to start reading
 * Returns:
 *  - where to continue from (first non-space point after start)
 */
static int skip_whitespace(bytes_t* buffer, int start) {
	// check inputs:
	assert(buffer != NULL);
	assert(start >= 0);
	assert(start < bytes_size(buffer));
	const int N = bytes_size(buffer);
	for (int i=start; i<N; i++) {
		if (isspace(bytes_get(buffer, i))) {
			continue;
		} else {
			return i;
		}
	}
	return N;
}

/* 
 * read_word
 * 
 * Inputs:
 *  - buffer: the string to read from
 *  - start: where to start reading
 * Outputs:
 *  - output: the bytes_t* to store the read word into.
 * Returns:
 *  - where to continue from (first space point after the word closest to start)
 * 
 */
static int read_word(bytes_t* buffer, int start, bytes_t* output) {
	// check inputs:
	assert(buffer != NULL);
	assert(start >= 0);
	assert(start < bytes_size(buffer));
	assert(output != NULL);

	// make space for output:
	bytes_clear(output);
	// skip whitespace:
	start = skip_whitespace(buffer, start);

	// grab the length:
	const int N = bytes_size(buffer);

	// read until the next space or end:
	for (int i=start; i<N; i++) {
		char c = bytes_get(buffer, i);
		if (isspace(c)) {
			return i;
		} else {
			bytes_push(output, c);
		}
	}
	return N;
}

/* 
 * read_maybe_quoted
 * 
 * Inputs:
 *  - buffer: the string to read from
 *  - start: where to start reading
 * Outputs:
 *  - output: the bytes_t* to store the read word into.
 * Returns:
 *  - where to continue from (first space point after the word closest to start)
 *  - OR (-1) on an error.
 * Errors:
 * 	- May not have close quote; may not have proper escape codes, etc.
 * 
 */
static int read_maybe_quoted(bytes_t* buffer, int start, bytes_t* output) {
	// check inputs:
	assert(buffer != NULL);
	assert(start >= 0);
	assert(start < bytes_size(buffer));
	assert(output != NULL);

	// make space for output:
	bytes_clear(output);
	// skip whitespace:
	start = skip_whitespace(buffer, start);

	// grab the length:
	const int N = bytes_size(buffer);

	// if no more space, done.
	if (start >= N) {
		return N;
	}
	// not-quoted case:
	if (bytes_get(buffer, start) != '"') {
		return read_word(buffer, start, output);
	}

	// read until the end quote or end:
	for (int i=start+1; i<N; i++) {
		char c = bytes_get(buffer, i);
		if (c == '\\') {
			// escape quotes and newlines:
			if (i+1 >= N) {
				fprintf(stderr, "Escape sequence found at end of file.\n");
				return -1;
			}
			i++; // step to next char
			char ec = bytes_get(buffer, i);
			if (ec == '\\' || ec == '"' || ec == '\'') {
				bytes_push(output, ec);
				continue;
			} else if (ec == 'n') {
				bytes_push(output, '\n');
			} else if (ec == 't') {
				bytes_push(output, '\t');
			} else if (ec == '0') {
				bytes_push(output, '\0');
			} else {
				fprintf(stderr, "Unknown escape sequence: \\%c\n", ec);
				return -1;
			}
		} else if (c == '"') {
			return i+1;
		} else {
			bytes_push(output, c);
		}
	}
	return N;
}

int main(void) {
	HashTable_t* dictionary = hash_new();

	bytes_t line_buffer; // stack-allocate buffer class
	bytes_init(&line_buffer); // initialize buffer variables

	while(true) {
		// Print prompt:
		printf("> "); 
		fflush(NULL);

		// Read data:
		ssize_t amount_read = bytes_readline(stdin, &line_buffer);
		if (amount_read == -1) {
			// EOF or quit:
			return 0;
		} 
		
		// Delete extraneous whitespace!
		bytes_trim(&line_buffer);

		// If they want to quit, let them.
		if (bytes_eqc(&line_buffer, "quit")) {
			return 0;
		}
		// If they didn't type anything, wait for next line of input.
		if (bytes_size(&line_buffer) == 0) {
			// didn't type anything!
			continue;
		}
		
		if (bytes_eqc(&line_buffer, "clear")) {
			// No printing needed.
			hash_clear(dictionary);
			continue;
		} else if (bytes_eqc(&line_buffer, "size")) {
			// Print hte size!
			printf("%zu\n", hash_size(dictionary));
			continue;
		}

		// all the rest of the commands need to be broken down:
		bytes_t* word = bytes_new_empty();
		// we keep track of how much of line we've read from in position:
		int position = read_word(&line_buffer, 0, word);
		int EOL = bytes_size(&line_buffer);
		if (position < 0 || position >= EOL) { 
			puts("ERROR");
			bytes_free(word);
			continue;
		 }

		if (bytes_eqc(word, "put")) {
			bytes_t* key = bytes_new_empty();
			bytes_t* value = bytes_new_empty();
			// read from there to the end of the key:
			position = read_maybe_quoted(&line_buffer, position, key);
			if (position < 0 || position >= EOL) { 
				puts("ERROR");
				bytes_free(key);
				bytes_free(value);
				bytes_free(word);
				continue;
			}
			// read from there to the end of the value:
			position = read_maybe_quoted(&line_buffer, position, value);
			if (position < 0) { 
				bytes_free(key);
				bytes_free(value);
				bytes_free(word);
				continue;
			}
			// insert: (key and value memory become owned by dictionary!)
			hash_put(dictionary, key, value);
		} else if (bytes_eqc(word, "get")) {
			bytes_t* key = bytes_new_empty();
			// read from there to the end of the key:
			position = read_maybe_quoted(&line_buffer, position, key);
			if (position < 0) { 
				puts("ERROR");
				bytes_free(key);
				bytes_free(word);
				continue;
			}
			// Look it up in the dictionary:
			bytes_t* maybe_found = hash_get(dictionary, key);
			if (maybe_found == NULL) {
				puts("NOT-FOUND");
			} else {
				printf("FOUND\n");
				bytes_println(stdout, maybe_found);
				printf("DONE\n");
			}
			// Release the key when we're done with it!
			bytes_free(key);
		} else if (bytes_eqc(word, "remove")) {
			bytes_t* key = bytes_new_empty();
			// read from there to the end of the key:
			position = read_maybe_quoted(&line_buffer, position, key);
			if (position < 0) { 
				bytes_free(key);
				bytes_free(word);
				continue;
			}
			// Remove from the dictionary:
			hash_remove(dictionary, key);
			// Release the key when we're done with it!
			bytes_free(key);
		}
		bytes_free(word);
	}

	// NOT going to let the OS clean up our data structures, this time.
  hash_clear(dictionary);
  free(dictionary);

	return 0;
}
