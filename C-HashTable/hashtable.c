#include "hashtable.h"
#include <math.h> // sqrt, round
#include <stdbool.h> // true, false
#include <stdlib.h> // calloc, free

// Define the smallest number of buckets allowable for a HashTable_t
const size_t smallest_num_buckets = 4;

// Construct a new HashTable (completed).
HashTable_t *hash_new(void) {
  HashTable_t *self = malloc(sizeof(HashTable_t));
  assert(self != NULL);
  self->num_used_buckets = 0;
  self->num_elements = 0;
  // CHALLENGE: have new HashTable_t objects start
  // with no memory until the first insert!
  self->num_buckets = 0;
  self->buckets = NULL;
  return self;
}

// Find the prime nearest a given number doubled or halved
// (depending on if growing or shrinking)
// Referenced this video and the idea of Erasthothenes' sieve:
// https://www.youtube.com/watch?v=r9mqm2dsFk4
static size_t find_prime(double num, bool growing) {
  // If growing, we will look for a prime near the doubled num,
  // going backwards
  if (growing) {
    // We know 2*num is not prime,
    // because the HashTable_t will always have
    // at least smallest_num_buckets != 1  slots.
    size_t x = (size_t)2 * num - 1;
    while (x != num) {
      bool isPrime = true;
      for (int mod = 2; mod <= (int)sqrt(x); mod++) {
        if (x % mod == 0) {
          isPrime = false;
          break;
        }
      }
      if (isPrime) {
        return x;
      }
      x--;
    }
    // If we could not find a prime,
    // just double the size of the original HashTable_t
    return (size_t)2 * num;
  }
  // If shrinking, we will look for a prime near the halved num,
  // going forward
  else {
    size_t x = round(num / 2.0);
    // Do not let the HashTable_t get too small!
    if (x < smallest_num_buckets) {
      return smallest_num_buckets;
    }
    while (x != num) {
      bool isPrime = true;
      for (int mod = 2; mod <= (int)sqrt(x); mod++) {
        if (x % mod == 0) {
          isPrime = false;
          break;
        }
      }
      if (isPrime) {
        return x;
      }
      x++;
    }
    // If we could not find a prime,
    // just half the size of the original HashTable_t
    return (size_t)round(num / 2.0);
  }
}

// Allocate a new array of buckets and migrate elements to new list (if any).
static void hash_resize(HashTable_t *self, bool growing) {
  // Allocate a new size for the array
  // The size will be bigger if growing and smaller if shrinking
  size_t new_num_buckets = find_prime((double)self->num_buckets, growing);
  size_t new_num_used = 0;
  HashEntry_t **new_buckets = calloc(new_num_buckets, sizeof(HashEntry_t *));
  assert(new_buckets != NULL);
  // Copy the elements from self->buckets into new_buckets
  for (size_t i = 0; i < self->num_buckets; i++) {
    HashEntry_t *current = self->buckets[i];
    HashEntry_t *next = NULL;
    while (current != NULL) {
      next = current->next;
      // Find the right bucket
      size_t bucket_num = current->hash % new_num_buckets;

      // Put current into the bucket
      HashEntry_t *bucket = new_buckets[bucket_num];
      // If necessary, increment the number of used buckets
      if (bucket == NULL) {
        new_num_used = new_num_used + 1;
      }
      current->next = bucket;
      new_buckets[bucket_num] = current;
      current = next;
    }
  }
  
  // Put the new data in self
  // Update number of buckets
  self->num_buckets = new_num_buckets;
  // Update number of used buckets
  self->num_used_buckets = new_num_used;
  // Update the buckets pointer in the HashTable_t and free the old pointer
  free(self->buckets);
  self->buckets = new_buckets;
}

// Return true if the hash table should resize (grow or shrink):
static bool hash_should_resize(HashTable_t *self) {
  double load = ((double)self->num_used_buckets) / ((double)self->num_buckets);
  if (load > .75 || (load < .35 && self->num_buckets != smallest_num_buckets)) {
    return true;
  }
  return false;
}

// Put a (key, value) pair into this list,
// overriding previous versions of the key if applicable.
void hash_put(HashTable_t *self, bytes_t *key, bytes_t *value) {
  assert(self != NULL);
  assert(key != NULL);
  assert(value != NULL);

  // Handle an uninitialized buckets pointer:
  if (self->buckets == NULL) {
    // Allocate a small number of buckets to start!
    // Note: by using calloc,
    // we avoid needing a for-loop to set entries to NULL!
    self->num_buckets = smallest_num_buckets;
    self->buckets = calloc(self->num_buckets, sizeof(HashEntry_t *));
    assert(self->buckets != NULL);
  }
  // Handle growing:
  else if (hash_should_resize(self)) {
    hash_resize(self, true);
  }

  int hash = bytes_hash(key);
  // Find the proper bucket for this HashEntry_t
  size_t bucket_num = hash % self->num_buckets;

  // See if this key already exists and we need to replace its value
  HashEntry_t *current = self->buckets[bucket_num];
  while (current != NULL && !bytes_equal(current->key, key)) {
    current = current->next;
  }
  // If the key is already in the HashTable_t, just replace its value
  if (current != NULL) {
    // Free the previous value
    bytes_free(current->value);
    current->value = value;
  }
  // If the key is not already in the HashTable_t,
  // create a HashEntry_t for this key and value and add it to the HashTable_t
  else {
    HashEntry_t *to_add = malloc(sizeof(HashEntry_t));
    assert(to_add != NULL);
    to_add->hash = hash;
    to_add->key = key;
    to_add->value = value;

    // Put to_add in the bucket
    HashEntry_t *bucket = self->buckets[bucket_num];
    // If necessary, increment the number of used buckets
    if (bucket == NULL) {
      self->num_used_buckets = self->num_used_buckets + 1;
    }
    to_add->next = bucket;
    self->buckets[bucket_num] = to_add;

    // Increment the number of elements in the HashTable_t
    self->num_elements = self->num_elements + 1;
  }
}

// Remove an entry matching this key in the association list.
void hash_remove(HashTable_t *self, bytes_t *key) {
  assert(self != NULL);
  assert(key != NULL);

  // Do not get an error if the buckets are not initialized
  if (self->buckets == NULL){
    return;
  }

  int hash = bytes_hash(key);
  // Find the right bucket
  size_t bucket = hash % self->num_buckets;
  // Find the right HashEntry_t
  HashEntry_t *previous = NULL;
  HashEntry_t *current = self->buckets[bucket];
  while (current != NULL && !bytes_equal(current->key, key)) {
    previous = current;
    current = current->next;
  }

  // Do not run into errors if the user asked to remove an element
  // that is not actually in HashTable_t
  if (current == NULL) {
    return;
  }

  // If necessary, decrement the number of used buckets
  if (previous == NULL && current->next == NULL) {
    self->num_used_buckets = self->num_used_buckets - 1;
  }

  // If it was the first node, just adjust the head
  if (previous == NULL) {
    self->buckets[bucket] = current->next;
  }
  // Otherwise, adjust the previous node's next
  else {
    previous->next = current->next;
  }
  // Free the space and decrement the HashTable_t's num_elements
  bytes_free(current->key);
  bytes_free(current->value);
  free(current);
  self->num_elements = self->num_elements - 1;

  // Handle shrinking:
  if (hash_should_resize(self)) {
    hash_resize(self, false);
  }
}

// Lookup an entry matching this key and return a pointer to its value.
bytes_t *hash_get(HashTable_t *self, bytes_t *key) {
  assert(self != NULL);
  assert(key != NULL);

  // Do not get an error if the buckets are not initialized
  if (self->buckets == NULL){
    return NULL;
  }
  
  int hash = bytes_hash(key);

  // Find the proper bucket for this key
  size_t bucket = hash % self->num_buckets;
  // Look through the HashEntry_t's in that bucket and
  // see if any have the same key; return the corresponding value
  HashEntry_t *current = self->buckets[bucket];
  while (current != NULL) {
    if (bytes_equal(current->key, key)) {
      return current->value;
    }
    current = current->next;
  }
  // Return NULL if this key is not in HashTable_t
  return NULL;
}

// Remove all entries from this list (try to free in the right places!)
void hash_clear(HashTable_t *self) {
  assert(self != NULL);

  for (size_t i = 0; i < self->num_buckets; i++) {

    // Traverse each bucket as a linked list; freeing stuff.
    HashEntry_t *current = self->buckets[i];
    HashEntry_t *next = NULL;
    while (current != NULL) {
      // grab next:
      next = current->next;
      // delete current:
      bytes_free(current->key);
      bytes_free(current->value);
      free(current);
      // move to next
      current = next;
    }

    // Start of list no longer points anywhere:
    self->buckets[i] = NULL;
  }

  // Finish clearing data:
  self->num_used_buckets = 0;
  self->num_elements = 0;
  self->num_buckets = 0;
  free(self->buckets);
  self->buckets = NULL;
}

// Lookup the number of elements in this data structure; prefer O(1) to O(n).
size_t hash_size(HashTable_t *self) {
  assert(self != NULL);
  return self->num_elements;
}
