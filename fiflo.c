#include "src/input.c"
#include "src/render.c"

uint32_t charBuffer, lineBuffer;

void argcCheck(int8_t argc) {
	if(argc > 2) {
		puts("Pass 0 or 1 (filename) arg to Fiflo.");
		exit(1);
	}
}

void typeAndPrint(void) {
	while(1) {
		int8_t pressedKey = unixGetch();
		if(pressedKey == 127) { // Backspace.
			charBuffer--;
		}
		else {
			charBuffer++;
		}
		clearWindow();
		window(pressedKey, charBuffer);
	}
}

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(uint8_t argc, int8_t *argv[]) {
	argcCheck(argc);
	window(' ', 0);
	typeAndPrint();
	return 0;
}

