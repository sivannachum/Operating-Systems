# OS S2020 Lab6 Starter

This starter code will be basically the same for P2.

## Where do I edit code?

- ``main.c`` and ``bytes.c`` should be perfect, but bugs are still possible.
- In theory, all you need to do is finish the data structure in ``assoc_list.c``.

## Unfinished methods/functions

(snippet from ``assoc_list.h``)

```c
void assoc_put(AssocList_t* self, bytes_t* key, bytes_t* value);
// Remove an entry matching this key in the association list.
void assoc_remove(AssocList_t* self, bytes_t* key);
// Lookup an entry matching this key and return a pointer to its value.
bytes_t* assoc_get(AssocList_t* self, bytes_t* key);
// Remove all entries from this list (try to free in the right places!)
```

## Important Types


### bytes_t - our String type

This is halfway between a Java ``String`` and ``ArrayList<Character>``.

(snippet from ``bytes.h``)
```c
typedef struct bytes_s {
  // heap-allocated byte-buffer.
	char *data;
  // the size of the buffer in data.
	size_t capacity;
  // the currently-used quantity of cells used in data.
	size_t len;
} bytes_t;
```


### AssocEntry_t - our Entry<String,String>

(snippet from ``assoc_list.h``)
```c
// Hash Entry type; key and value as bytes_t.
typedef struct AssocEntry_s {
	// The key in the entry; a byte-string.
	bytes_t* key;
	// The value in the entry; a byte-string; usually not relevant to the data structure.
	bytes_t* value;
	// A chaining link to the next entry (maybe NULL!)
	struct AssocEntry_s* next;
} AssocEntry_t;
```


### AssocList_t - our Map<String,String>

(snippet from ``assoc_list.h``)
```c
// List of Entries pretending to be a Map; actually a Linked List!
typedef struct AssocList_s {
	// The number of entries in this list; stored here for O(1).
	size_t num_elements;
	// The head of the list; NULL if empty!
	AssocEntry_t *head;
} AssocList_t;
```

## Building with a Makefile

- ``make test`` executes all build commands to run first ``test_bytes.c`` and then ``test_assoc.c``
- ``make run`` runs the interactive ``main.c``
- ``make debug`` debugs the interactive ``main.c`` --> will run it via gdb so you get line numbers from segfaults.
- ``make clean`` will delete all temporary build files.

## Using CMake instead

CMake doesn't come with the ability to create actions; traditionally you give it its own build directory to do its thing. This will port over to Windows and other platforms better than the older ``Makefile`` approach.

```bash
cd build && cmake ..
make test_assoc && ./test_assoc
make test_bytes && ./test_bytes
make main && ./main
```
