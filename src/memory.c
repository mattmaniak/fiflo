#include "memory.h"

void memError(void) {
	fputs("Cannot allocate memory!\n", stderr);
	exit(1);
}

