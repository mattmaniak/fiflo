// Memory && IO operations.

#include "hardware.h"

void pointerCheck(void *ptr, char *errmsg) // Eg. malloc or FILE*.
{
	if(ptr == NULL)
	{
		fprintf(stderr, "%s\n", errmsg);
		exit(1);
	}
}

void sigstpHandler(int nothing)
{
	puts("Process set to background, use 'fg', to get it back.");
	if(nothing == 0) {}
}

void emptyHandler(int nothing) // For "‘__sighandler_t {aka void (*)(int)}".
{
	if(nothing == 0) {}
}

char unixGetch(void) // https://stackoverflow.com/questions/12710582/
{
	char key;
	struct termios oldt, newt;

	tcgetattr(STDIN_FILENO, &oldt); // Get terminal attribiutes.
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Set terminal attribiutes.

	key = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Set terminal attribiutes.
	return key;
}

