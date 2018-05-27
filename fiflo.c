#include "src/render.h"

// *argv[] - table of pointers, (*argv)[] pointer of table.
int main(unsigned char argc, char *argv[]) {
	if(argc > 2) {
		exit(1);
	}

	window();
	return 0;
}

