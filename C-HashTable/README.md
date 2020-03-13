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
