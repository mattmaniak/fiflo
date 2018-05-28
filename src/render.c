#include "render.h"

#define MIN_WIDTH 80
#define MIN_HEIGHT 20
#define MAX_CHAR_AMOUNT 0x7FFFFFFF
#define whiteblock "\033[7m \033[0m"

uint16_t i;
uint16_t windowSize(int8_t axis)
{
	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
	if(win.ws_col < MIN_WIDTH || win.ws_row < MIN_HEIGHT)
	{
		puts("Minimal terminal size is 40x20.");
		exit(1);
	}

	if(axis == 'x')
	{
		return win.ws_col;
	}
	else if(axis == 'y')
	{
		return win.ws_row;
	}
}

void clearWindow(void) // To provide rendering in a one frame.
{
	uint16_t winHeight = windowSize('y');
	for(i = 0; i < winHeight; i++)
	{
		printf("%s", "\033[F\033[K");
	}
}

void window(int8_t pressedKey, int32_t charCount)
{
	{
		if(charCount <= 0)
		{
			charCount = 0;
		}
		else if(charCount >= MAX_CHAR_AMOUNT)
		{
			charCount = MAX_CHAR_AMOUNT;
		}	
	}

	uint16_t winWidth = windowSize('x');
	uint16_t winHeight = windowSize('y');

	int8_t programName[7] = " Fiflo ";
	uint16_t programNameLen = strlen(programName);
	uint16_t programNameCenter = (winWidth - programNameLen) / 2;
	for(i = 0; i < programNameCenter; i++)
	{
		printf("%s", whiteblock);
	}

	printf("%s", programName);
	if(winWidth % 2 == 0)
	{
		for(i = 0; i <= programNameCenter; i++)
		{
			printf("%s", whiteblock);
		}
	}
	else
	{
		for(i = 0; i < programNameCenter; i++)
		{
			printf("%s", whiteblock);
		}
	}

	printf("%c", pressedKey);
	for(i = 3; i < winHeight; i++)
	{
		printf("%c", '\n');
	}

	printf("%s", "Chars: ");
	printf("%i", charCount);
	printf("%c", '\n');
	for(i = 0; i < winWidth; i++)
	{
		printf("%s", whiteblock);
	}
}

