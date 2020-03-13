#ifndef __ASSOC_LIST_H
#define __ASSOC_LIST_H 1

#include "bytes.h"

// Hash Entry type; key and value as bytes_t.
typedef struct HashEntry_s {
	// The hash-code of the key; stored for efficiency.
	int hash;
	// The key in the entry; a byte-string.
	bytes_t* key;
	// The value in the entry; a byte-string; usually not relevant to the data structure.
	bytes_t* value;
	// A chaining link to the next entry (maybe NULL!)
	struct HashEntry_s* next;
} HashEntry_t;

// List of Entries pretending to be a Map; actually a Linked List!
typedef struct HashTable_s {
	// The number of entries in this table; stored here for O(1).
	size_t num_elements;
	// The number of buckets in use; stored here for O(1).
	size_t num_used_buckets;
	// The number of bucket entries; needed to accompany the buckets array below:
	size_t num_buckets;
	// An array of pointers to entries.
	HashEntry_t **buckets;
} HashTable_t;


// Construct a new association list
HashTable_t* hash_new(void);
// Put a (key, value) pair into this list, overriding previous versions of the key if applicable.
void hash_put(HashTable_t* self, bytes_t* key, bytes_t* value);
// Remove an entry matching this key in the association list.
void hash_remove(HashTable_t* self, bytes_t* key);
// Lookup an entry matching this key and return a pointer to its value.
bytes_t* hash_get(HashTable_t* self, bytes_t* key);
// Remove all entries from this list (try to free in the right places!)
void hash_clear(HashTable_t* self);
// Lookup the number of elements in this data structure; prefer O(1) to O(n).
size_t hash_size(HashTable_t* self);

#endif
