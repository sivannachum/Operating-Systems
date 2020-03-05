#ifndef __ASSOC_LIST_H
#define __ASSOC_LIST_H 1

#include "bytes.h"

// Hash Entry type; key and value as bytes_t.
typedef struct AssocEntry_s {
	// The key in the entry; a byte-string.
	bytes_t* key;
	// The value in the entry; a byte-string; usually not relevant to the data structure.
	bytes_t* value;
	// A chaining link to the next entry (maybe NULL!)
	struct AssocEntry_s* next;
} AssocEntry_t;

// List of Entries pretending to be a Map; actually a Linked List!
typedef struct AssocList_s {
	// The number of entries in this list; stored here for O(1).
	size_t num_elements;
	// The head of the list; NULL if empty!
	AssocEntry_t *head;
} AssocList_t;

// Construct a new association list
AssocList_t* assoc_new(void);
// Put a (key, value) pair into this list, overriding previous versions of the key if applicable.
void assoc_put(AssocList_t* self, bytes_t* key, bytes_t* value);
// Remove an entry matching this key in the association list.
void assoc_remove(AssocList_t* self, bytes_t* key);
// Lookup an entry matching this key and return a pointer to its value.
bytes_t* assoc_get(AssocList_t* self, bytes_t* key);
// Remove all entries from this list (try to free in the right places!)
void assoc_clear(AssocList_t* self);
// Lookup the number of elements in this data structure; prefer O(1) to O(n).
size_t assoc_size(AssocList_t* self);

#endif
