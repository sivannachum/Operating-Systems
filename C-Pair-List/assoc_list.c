#include "assoc_list.h"
#include <stdlib.h> // malloc, free

// Construct a new association list (completed).
AssocList_t* assoc_new(void) {
	AssocList_t* self = malloc(sizeof(AssocList_t));
	assert(self != NULL);
	self->num_elements = 0;
	self->head = NULL;
	return self;
}

// Put a (key, value) pair into this list, overriding previous versions of the key if applicable.
void assoc_put(AssocList_t* self, bytes_t* key, bytes_t* value) {
	assert(self != NULL);
	assert(key != NULL);
	assert(value != NULL);

	// Loop through the list to check if key is already in there
	for (AssocEntry_t *curr = self->head; curr != NULL; curr = curr->next){
	  // If the key is already in the list, replace its value
	  if (bytes_equal(curr->key, key)){
	    bytes_free(curr->value);
	    curr->value = value;
	    return;
          }
        }

	// Otherwise, this is a new key; create a new AssocEntry_t for it,
	// and put it at the head of the AssocList_t
	AssocEntry_t *curr = malloc(sizeof(AssocEntry_t));
	assert(curr != NULL);
	curr->key = key;
	curr->value = value;
	curr->next = self->head;
	self->head = curr;
	self->num_elements = self->num_elements + 1;

}

// Remove an entry matching this key in the association list.
void assoc_remove(AssocList_t* self, bytes_t* key) {
	assert(self != NULL);
	assert(key != NULL);

	// Look for the node to remove
	AssocEntry_t *curr = self->head;
	AssocEntry_t *prev = NULL;
	while (curr != NULL && !bytes_equal(curr->key, key)){
	  curr = curr->next;
	  prev = curr;
	}

	// If it was the first node, just adjust the head and free the space
	if (prev == NULL && curr != NULL){
	  self->head = curr->next;
	  bytes_free(curr->key);
	  bytes_free(curr->value);
	  free(curr);
	  self->num_elements = self->num_elements - 1;
	}
	// If it was a different node,
	// adjust the previous node's next and free the space
	else if (curr != NULL){
	  prev->next = curr->next;
	  bytes_free(curr->key);
	  bytes_free(curr->value);
	  free(curr);
	  self->num_elements = self->num_elements - 1;
	}
}

// Lookup an entry matching this key and return a pointer to its value.
bytes_t* assoc_get(AssocList_t* self, bytes_t* key) {
	assert(self != NULL);
	assert(key != NULL);

	// Look for the key in the list; return the corresponding value
	for (AssocEntry_t *curr = self->head; curr != NULL; curr = curr->next){
	  if (bytes_equal(curr->key, key)){
	    return curr->value;
	  }
	}
	
	// Return NULL if this key is not in AssocList_t
	return NULL;
}

// Remove all entries from this list (try to free in the right places!)
void assoc_clear(AssocList_t* self) {
	assert(self != NULL);

	AssocEntry_t* current = self->head;
	AssocEntry_t* next = NULL;
	while(current != NULL) {
		// grab next:
		next = current->next;
		// delete current:
		bytes_free(current->key);
		bytes_free(current->value);
		free(current);
		// move to next
		current = next;
	}

	// Finish clearing data:
	self->num_elements = 0;
	self->head = NULL;
}

// Lookup the number of elements in this data structure; prefer O(1) to O(n).
size_t assoc_size(AssocList_t* self) {
	assert(self != NULL);
	return self->num_elements;
}
