CC=gcc
CFLAGS=-Wall -Werror -g -pipe

.PHONY: run
run: main
	./main

.PHONY: debug
debug: main
	gdb -q main -ex run

.PHONY: clean
clean:
	rm -rf test_bytes test_assoc main *.o

.PHONY: test
test: test_bytes test_assoc
	./test_bytes
	./test_assoc

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

test_bytes: bytes.o test_bytes.o
	$(CC) $^ -o $@

test_assoc: bytes.o hashtable.o test_assoc.o
	$(CC) $^ -o $@ -lm

main: main.o bytes.o hashtable.o
	$(CC) $^ -o $@ -lm

