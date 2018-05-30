#include "src/input.c"
#include "src/render.c"

#define MAX_CHAR_AMOUNT 0x7FFFFFFF

int32_t charBuffer = 0;

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
			if(charBuffer <= 0) {
				charBuffer = 0;
			}
		}
		else {
			charBuffer++;
			if(charBuffer >= MAX_CHAR_AMOUNT) {
				charBuffer = MAX_CHAR_AMOUNT;
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

