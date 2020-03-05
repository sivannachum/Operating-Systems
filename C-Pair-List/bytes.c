#include "bytes.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

static void ensure_length(bytes_t *self, size_t n) {
  assert(self != NULL);
  if (self->capacity > n) {
    return;
  }
  if (self->data == NULL) {
    self->capacity = 4;
  }
  while (self->capacity < n) {
    self->capacity *= 2;
  }
  self->data = realloc(self->data, self->capacity);
  // Make sure allocation worked!
  assert(self->data);
}

// Copy the data from a subset of a cstring into this bytes object.
static bytes_t *bytes_copy_slice(char *data, size_t len) {
  assert(data != NULL);
  bytes_t *buf = bytes_new_empty();
  ensure_length(buf, len);
  buf->len = len;
  for (int i = 0; i < len; i++) {
    buf->data[i] = data[i];
  }
  return buf;
}

// Allocate a new bytes object that is empty, on the heap.
bytes_t *bytes_new_empty(void) {
  bytes_t *new = malloc(sizeof(bytes_t));
  bytes_init(new);
  return new;
}
// Copy the data from a cstring into this bytes object.
bytes_t *bytes_copy_str(char *data) {
  assert(data != NULL);
  return bytes_copy_slice(data, strlen(data));
}

// Delete a bytes object:
void bytes_clear(bytes_t *self) {
  assert(self != NULL);
  if (self->capacity > 0) {
    free(self->data);
    self->capacity = 0;
    self->len = 0;
    self->data = NULL;
  }
}

// Free all of a bytes object:
void bytes_free(bytes_t *self) {
  assert(self != NULL);
  bytes_clear(self);
  free(self);
}

// Initialize a new bytes object.
void bytes_init(bytes_t *self) {
  assert(self != NULL);
  self->data = NULL;
  self->len = 0;
  self->capacity = 0;
}

// Add a character to the back of this string (may reallocate).
void bytes_push(bytes_t *self, char x) {
  assert(self != NULL);
  ensure_length(self, self->len + 1);
  self->data[self->len++] = x;
}

// Remove a character from the back of this string (-1 if none!);
int bytes_pop(bytes_t *self) {
  assert(self != NULL);
  if (self->len == 0) {
    return -1;
  }
  return self->data[self->len--];
}

// Access the size of this bytes object.
size_t bytes_size(bytes_t *self) {
  assert(self != NULL);
  return self->len;
}

// Get the character at a given index (safely).
char bytes_get(bytes_t* self, int index) {
	assert(self != NULL);
	assert(index < self->len);
	assert(index >= 0);
	return self->data[index];
}

// Hash the string stored inside this bytes_t:
int bytes_hash(bytes_t* self) {
	assert(self != NULL);

	// Using Java's algorithm:
	int hash = 7;

	// Mix in every character with primes.
	for (size_t i=0; i<self->len; i++) {
		hash = (hash * 31) + self->data[i];
	}

	return hash;
}

// Get rid of whitespace on the right-hand-side of the string.
static void bytes_trim_end(bytes_t *self) {
  for (ssize_t i = self->len - 1; i >= 0; i--) {
    if (isspace(self->data[i])) {
      self->len--;
    } else {
      break;
    }
  }
}

// Get rid of whitespace on the left-hand-side of the string.
static void bytes_trim_start(bytes_t *self) {
  size_t first_non_space = 0;
  for (size_t i = 0; i < self->len; i++) {
    first_non_space = i;
    if (isspace(self->data[i])) {
      continue;
    } else {
      break;
    }
  }
  size_t new_len = self->len - first_non_space;
  if (new_len == self->len) {
    return;
  }
  // slide-to-the-left
  for (size_t dest = 0; dest < new_len; dest++) {
    size_t src = dest + first_non_space;
    self->data[dest] = self->data[src];
  }
  self->len = new_len;
}

void bytes_trim(bytes_t *self) {
	assert(self != NULL);
	bytes_trim_end(self);
	bytes_trim_start(self);
}


void bytes_debug(bytes_t *self) {
  printf("bytes object: %p", self);
  if (self == NULL) {
    printf("  .. is NULL; be careful!");
    return;
  }
  printf("  capacity = %zu\n  len = %zu\n", self->capacity, self->len);
  printf("  data = ||");
  bytes_print(stdout, self);
  printf("||\n");
}

void bytes_print(FILE *fp, bytes_t *self) {
  for (size_t i = 0; i < self->len; i++) {
    fprintf(fp, "%c", self->data[i]);
  }
}

void bytes_println(FILE *fp, bytes_t *self) {
  bytes_print(fp, self);
  fprintf(fp, "\n");
}

bool bytes_equal(bytes_t* self, bytes_t* other) {
  assert(self != NULL);
  assert(other != NULL);
  if (self->len != other->len) {
    return false;
  }
  for (int i=0; i<self->len; i++) {
		if (other->data[i] != self->data[i]) {
      return false;
    }
  }
  return true;
}

// Is this the same as the C-String cmp?
bool bytes_eqc(bytes_t *self, char *cmp) {
	size_t i;
  assert(self != NULL);
  assert(cmp != NULL);

  for (i = 0; i < self->len; i++) {
    if (cmp[i] == '\0') {
      // when we reach the end of the comparison string without hitting the end
      // of our bytes-str. we better see that in our bytes-array, too and it
      // better be the end (or they're not the same!)
      return cmp[i] == self->data[i] && i + 1 >= self->len;
    }
		if (cmp[i] != self->data[i]) {
			return false;
		}
  }
  // better be done with comparison string...
  return cmp[i] == '\0';
}

ssize_t bytes_readline(FILE* fp, bytes_t* self) {
	// "clear" the data but don't re-allocate
	self->len = 0;

	while(true) {
		int next = fgetc(fp);
		if (next == EOF) {
			return -1;
		} else if (next == '\n') {
			return bytes_size(self);
		}
		// keep whatever they type:
		bytes_push(self, next);
	}
}

bool bytes_startswith(bytes_t* self, char* pattern) {
	size_t len = strlen(pattern);
	if (bytes_size(self) < len) {
		return false;
	}
	// this is basically strncmp:
	for (size_t i=0; i<len; i++) {
		if (bytes_get(self, i) != pattern[i]) {
			return false;
		}
	}
	return true;
}
