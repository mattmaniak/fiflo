#include "src/input.c"
#include "src/render.c"

// Chars amount: from 0 to signed int8/16/32_t - 1.
#define CHAR_BUFFER_SIZE 0x7F - 0x1 // Scope: <0; 126>.

int8_t charsAmount = 0, linesAmount = 1;
// TODO int8_t textBuffer[1][80];

void typeAndPrint(void) {
	while(1) {
		char pressedKey = unixGetch();
		if(pressedKey == 127) { // Backspace.
			charsAmount--;
			if(charsAmount <= 0) {
				charsAmount = 0;
			}
		}
		else {
			charsAmount++;
			if(charsAmount >= CHAR_BUFFER_SIZE) {
				charsAmount = CHAR_BUFFER_SIZE;
			}
		}
		cleanFrame();
		window(charsAmount, linesAmount, pressedKey);
	}
}

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(void) {
	windowEmpty(charsAmount, linesAmount);
	typeAndPrint();
	return 0;
}

