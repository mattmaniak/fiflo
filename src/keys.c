#include "keys.h"

char text[19][BUFF_SIZE];

void keyHandling(BUFF_T lines, BUFF_T chars, char key)
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

