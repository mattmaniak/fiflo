#include "hardware.h"

// IO and memory operations.

void memError(void) {
	fputs("Cannot allocate memory!\n", stderr);
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

