#include "src/input.c"
#include "src/render.c"

// Chars amount: from 0 to signed int8/16/32_t - 1.
#define CHAR_BUFFER_SIZE 0x7F - 0x1 // Scope: <0; 126>.

int8_t charAmount = 1;

void typeAndPrint(char filename[32]) {
	int8_t filenameSize = strlen(filename);
	if(filenameSize > 32) {
		puts("\nMaximum filename lenght is 32 chars.");
		exit(1);
	}
	while(1) {
		char pressedKey = unixGetch();
		if(pressedKey == 127) { // Backspace.
			charAmount--;
			if(charAmount <= 0) {
				charAmount = 0;
			}
		}
		else {
			charAmount++;
			if(charAmount >= CHAR_BUFFER_SIZE) {
				charAmount = CHAR_BUFFER_SIZE;
			}
		}
		clearFrame();
		window(pressedKey, charAmount, filename);
	}
}

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(int argc, char *argv[]) {
	if(argc != 2) {
		puts("Pass certainly 1 arg (filename) to Fiflo.");
		exit(1);
	}
	window('s', charAmount, argv[1]);
	typeAndPrint(argv[1]);
	return 0;
}

