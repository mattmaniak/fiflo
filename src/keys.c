#include "keys.h"

#define MAX_BUFFER_SIZE 126 // Always [TYPE]_MAX - 1.
static char text[19][80];

void keyHandling(int8_t lines, int8_t chars, char key)
{
	if(key != CTRL_B) // To prevent double 'backspace'.
	{
		text[lines - 1][chars - 1] = key; // Allocation.
		text[lines - 1][chars] = '\b';
	}
}

