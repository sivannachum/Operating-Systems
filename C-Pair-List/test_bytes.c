#include "bytes.h"
#include <assert.h>

int main(int argc, char **argv) {
	bytes_t* foo = bytes_new_empty();
	bytes_push(foo, 'x');
	bytes_push(foo, 'y');
	assert(bytes_eqc(foo, "xy"));
	assert(bytes_get(foo, 0) == 'x');
	assert(bytes_get(foo, 1) == 'y');

	for (int i=0; i<9; i++) {
		bytes_push(foo, 'a'+i);
	}
	assert(bytes_eqc(foo, "xyabcdefghi"));
	bytes_debug(foo);
	
	// delete bytes and debug again:
	bytes_clear(foo);
	bytes_debug(foo);

	bytes_t* wasted_space = bytes_copy_str("\tindented and extra \n");
	bytes_debug(wasted_space);
	
	bytes_trim(wasted_space);
	bytes_debug(wasted_space);
	assert(bytes_eqc(wasted_space, "indented and extra"));
}
