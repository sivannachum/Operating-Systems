#ifndef _BYTES_H
#define _BYTES_H 1

#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

// Declare a "String" type.
typedef struct bytes_s {
	char *data;
	size_t capacity;
	size_t len;
} bytes_t;

// Allocate a new bytes object that is empty, on the heap.
bytes_t* bytes_new_empty(void);
// Copy the data from a cstring into this bytes object.
bytes_t* bytes_copy_str(char* data);

// Delete the insides of a bytes object:
void bytes_clear(bytes_t* self);
// Free all of a bytes object:
void bytes_free(bytes_t* self);

// Initialize a new bytes object.
void bytes_init(bytes_t* self);
// Add a character to the back of this string (may reallocate).
void bytes_push(bytes_t* self, char x);
// Remove a character from the back of this string (-1 if none!);
int bytes_pop(bytes_t* self);
// Access the size of this bytes object.
size_t bytes_size(bytes_t* self);
// Get the character at a given index (safely).
char bytes_get(bytes_t* self, int index);
// Hash the string stored inside this bytes_t:
int bytes_hash(bytes_t* self);

// Get rid of whitespace on the sides of the string.
void bytes_trim(bytes_t* self);

// Debug a bytes struct! (works on NULL pointers!)
void bytes_debug(bytes_t* self);
// Print the characters inside to a file!
void bytes_print(FILE* fp, bytes_t* self);
// Print the characters inside to a file with a newline!
void bytes_println(FILE* fp, bytes_t* self);

// Read a line from a file into self, return the number of bytes read.
ssize_t bytes_readline(FILE* fp, bytes_t* self);

// Are these bytes objects the same?
bool bytes_equal(bytes_t *self, bytes_t* other);
// Is this the same as the C-String cmp?
bool bytes_eqc(bytes_t* self, char* cmp);
// Does this bytes_t start with the C-String pattern?
bool bytes_startswith(bytes_t* self, char* pattern);

#endif
