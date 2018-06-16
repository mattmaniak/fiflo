#include "keys.h"

#define MAX_BUFFER_SIZE 126 // Always [TYPE]_MAX - 1.
char text[19][80];

void keyHandling(int8_t lines, int8_t chars, char key)
{
	if(key == BACKSPACE) // To prevent double 'backspace'.
	{
		text[lines - 1][chars] = '\0';
	}
	else
	{
		text[lines - 1][chars - 1] = key; // Allocation.
		text[lines - 1][chars] = '\0';

	}
}

