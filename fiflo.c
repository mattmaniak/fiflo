#include "src/render.h"

/* TODO
void round(void) {
	while(1) {
		if(pressed key) {		
			window(); }}} */

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(uint8_t argc, int8_t *argv[]) {
	if(argc > 2) {
		exit(1); }

	int8_t chr = getchar();
	printf("%c", chr);
	window();
	return 0; }

