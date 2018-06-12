#include "hardware.h"

// Memory && IO operations.

void pointerCheck(void* pointer) // Empty pointer checker.
{
	if(pointer == NULL)
	{
		fputs("Pointer error!\n", stderr);
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

void saveToFile(char base_filename[])
{
	FILE* textfile = fopen(base_filename, "rw");
	pointerCheck(textfile);

	// TODO

	fclose(textfile);
}

