#include "src/headers.h"
#include "src/input.h"
#include "src/render.h"

void typeAndPrint(void) {
	while(1) {
		int8_t pressedKey = unixGetch();
		window(pressedKey); }}

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(uint8_t argc, int8_t *argv[]) {
	if(argc > 2) {
		exit(1); }

	window(' ');
	typeAndPrint();
	return 0; }

