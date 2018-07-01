// Memory && IO operations.

#include "hardware.h"

// TODO: TAKE IT FROM HERE!
void pointerCheck(void *ptr, char *errmsg) // Eg. malloc or FILE*.
{
	if(ptr == NULL)
	{
		fprintf(stderr, "%s\n", errmsg);
		exit(1);
	}
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

