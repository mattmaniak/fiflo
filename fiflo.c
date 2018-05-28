#include "src/headers.h"
#include "src/input.h"
#include "src/keymap.h"
#include "src/render.h"

uint32_t charCount;

void argcCheck(int8_t argc)
{
	if(argc > 2)
	{
		puts("Pass 0 or 1 (filename) arg to Fiflo.");
		exit(1);
	}
}

void typeAndPrint(void)
{
	while(1)
	{
		int8_t pressedKey = unixGetch();
		charCount++;
		clearWindow();
		window(pressedKey, charCount);
	}
}

// *asdf[] - table of pointers, (*asdf)[] pointer to table.
int main(uint8_t argc, int8_t *argv[])
{
	argcCheck(argc);

	window(' ', 0);
	typeAndPrint();
	return 0;
}

