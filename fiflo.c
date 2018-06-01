#include "src/input.c"
#include "src/render.c"

// Chars amount: from 0 to signed int8/16/32_t - 1.
#define CHAR_BUFFER_SIZE 0x7F - 0x1 // Scope: <0; 126>.

int8_t charBuffer = 0;

void typeAndPrint(char filename[32]) {
	int8_t filenameSize = strlen(filename);
	if(filenameSize > 32) {
		puts("\nMaximum filename lenght is 32 chars.");
		exit(1);
	}
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
		clearFrame();
		window(pressedKey, charBuffer);
	}
}

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(int argc, char *argv[]) {
	if(argc != 2) {
		puts("Pass certainly 1 arg (filename) to Fiflo.");
		exit(1);
	}
	printf("%s", "\033[F\033[K");
	printf("Filename: %s", argv[1]);
	window(' ', charBuffer);

	typeAndPrint(argv[1]);
	return 0;
}

