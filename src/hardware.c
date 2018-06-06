#include "hardware.h"

// Memory && IO operations.

void memError(void) {
	fputs("\nCannot allocate memory!\n", stderr);
	exit(1);
}

void fileError(void) {
	fputs("\nCannot open the file!\n", stderr);
	exit(1);
}

char unixGetch(void) { // https://stackoverflow.com/questions/12710582/
	char key;
	struct termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	key = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return key;
}

void writeToFile(void) {
	FILE* selectedFile = fopen("textfile.asdf", "r+");
	if(selectedFile == NULL) {
		selectedFile = fopen("textfile.asdf", "wb");

		if(selectedFile == NULL) {
			fileError();
		}
	}
	char *text = "Write this to the file";
	fprintf(selectedFile, "%s\n", text);
	fclose(selectedFile);
}

