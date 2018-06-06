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

