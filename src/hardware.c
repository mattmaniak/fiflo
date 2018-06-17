// Memory && IO operations.

#include "hardware.h"

void pointerCheck(void* pointer) // Eg. malloc or FILE*.
{
	if(pointer == NULL)
	{
		fputs("Memory pointer error.\n", stderr);
		exit(1);
	}
}

char unixGetch(void) // https://stackoverflow.com/questions/12710582/
{
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

