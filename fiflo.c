#include "src/input.c"
#include "src/render.c"

// Chars amount: from 0 to signed int8/16/32_t - 1.
#define CHAR_BUFFER_SIZE 0x7F - 0x1 // Scope: <0; 126>.

int8_t charsAmount = 0, linesAmount = 1;

void typeAndPrint(char filename[32]) {
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
		window(pressedKey, filename, charsAmount, linesAmount);
	}
}

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(int argc, char *argv[]) {
	if(argc != 2) {
		fputs("Usage: Fiflo [filename].\n", stderr);
		exit(1);
	}
	if(argv[1] == "-h" || argv[1] == "--help") { // TODO
		help();
		exit(0);
	}
	int8_t filenameLen = strlen(argv[1]);
	if(filenameLen > 32) {
		fputs("Maximum filename length is 32 chars.\n", stderr);
		exit(1);
	}
	windowEmpty(argv[1], charsAmount, linesAmount);
	typeAndPrint(argv[1]);
	return 0;
}

