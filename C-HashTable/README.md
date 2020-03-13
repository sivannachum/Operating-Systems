# OS S2020 Lab6 Starter

This starter code will be basically the same for P2.

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

## Rubric

### Completing put and get (40):

Right now the ``hash_new`` method creates a HashTable_t with 8 buckets (pointers to HashEntry_t). This is enough buckets (1 is enough) to pass any of the tests.

Having functional ``hash_put`` and ``hash_get`` methods are worth 40 points; they are the most critical.

If you're struggling with the ``HashEntry_t** buckets``; remember it is an array of pointers -- these are pointers to the head of a linked-list. 

You can import your ``lab6`` code and change these buckets to be ``AssocList_t* buckets``; treating the collision handling part of your code as delegated to your prior assignment. This will involve slightly modifying the ``Makefile`` or ``CMakeLists.txt``.

### Completing remove (10):

Since ``hash_remove`` is the most difficult method, it is worth additional points.

### Growing HashTable_t (20+):

There are two ``private`` methods in ``hashtable.c``; ``static`` on a function in ``C`` is like ``private`` in Java -- it means that the function is only visible in the current file.

 - ``boolean hash_should_resize(HashTable_t *self)`` should be complete already.
 - (12) ``void hash_resize(HashTable_t* self)`` needs work. I suggest using ``calloc`` to create another new array, since it fills it with zeros (NULLs).
 - (8) Also, you'll need to fiddle with ``test_assoc`` in order to trigger your ``HashTable_t`` to resize!

Extra points will be available for:
 
 - (5) Resizing to only numbers of buckets that are reasonably prime (e.g., ``(2 ** x) - 1``) ... I don't expect you to come up with this algorithm by yourself - cite your googling.
 - (?) Not relying on ``hash_put`` to do this logic; it is much more efficient if you're storing ``hash`` values in your entries to duplicate some logic here.

### Shrinking HashTable_t (10):

This is much like the previous section, but more points will be available for re-using code as much as possible.

### Memory Correctness and Style (20):

The last 20 points of this assignment are for memory correctness and style -- you may earn all of the previous listed points with memory leaks; however, this is intended to be a long running program, so think of ways to manage memory safely.

Similarly, some of these points will be awarded for clearer, simpler solutions.

Reading the section in "Dive into Systems" on ``valgrind`` will provide you with tools to help track the leaking memory.