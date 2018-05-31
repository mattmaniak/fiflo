#include "src/input.c"
#include "src/render.c"

// Chars amount: from 0 to signed int8/16/32_t - 1.
#define CHAR_BUFFER_SIZE 0x7F - 0x1 // Scope: <0; 126>.

int8_t charBuffer = 0;

void argcCheck(char argc) {
	if(argc > 2) {
		puts("Pass 0 or 1 (filename) arg to Fiflo.");
		exit(1);
	}
}

void typeAndPrint(void) {
	while(1) {
		char pressedKey = unixGetch();

		if(pressedKey == 127) { // Backspace.
			charBuffer--;
			if(charBuffer <= 0) {
				charBuffer = 0;
			}
		}
		else {
			charBuffer++;
			if(charBuffer >= CHAR_BUFFER_SIZE) {
				charBuffer = CHAR_BUFFER_SIZE;
			}
		}
		clearWindow();
		window(pressedKey, charBuffer);
	}
}

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(uint8_t argc, int8_t *argv[]) {
	argcCheck(argc);
	window(' ', charBuffer);
	typeAndPrint();
	return 0;
}

