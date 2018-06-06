#include "src/hardware.c"
#include "src/render.c"

// File with the main logic.

// Chars amount: from 0 to signed int8/16/32_t - 1.
#define CHAR_BUFFER_SIZE 0x7F - 0x1 // Scope: <0; 126>.

int8_t charsAmount = 0, linesAmount = 1;

void programRound(void) {
	while(1) {
		char pressedKey = unixGetch();
		if(pressedKey == BACKSPACE) {
			charsAmount--;
			if(charsAmount <= 0) {
				charsAmount = 0;
			}
		}
		else if(pressedKey == CTRL_X) {
			exit(0);
		}
		else {
			charsAmount++;
			if(charsAmount >= 80) { // TODO: the last char is overwritten.
				charsAmount = 80;
			}
		}
		cleanFrame();
		window(charsAmount, linesAmount, pressedKey);
	}
}

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(int argc, char* argv[]) {
	if(argc > 2) {
		fputs("Usage: fiflo [base filename-only-for-WIP]\n", stderr);
		exit(1);
	}
	if(argv[1] == NULL) {
		fputs("Usage: fiflo [base filename-only-for-WIP]\n", stderr);
		exit(1);
	}
	window(charsAmount, linesAmount, '\0');
	programRound();
	return 0;
}

