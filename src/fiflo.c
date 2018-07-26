#ifdef __linux__

#include "fiflo.h"

#include "hardware.c"
#include "logic.c"

void showHelp(void)
{
	printf("%s\n%s\n%s\n%s\n%s\n",
	"Usage: fiflo [option].",
	"Options:     Description:",
	"<NULL>       Create and open the default file - 'noname.asdf'.",
	"<file>       Open the textfile named 'file'.",
	"-h, --help   Show program help.");
}

void run(char *name)
{
	struct Data buff = readFile(buff, name);
	char pressed_key = TERMINATOR;

	window(buff, pressed_key);
	printf("%s%c%s", BLINK, '_', RESET); // Init cursor placeholder.
	windowFill(buff.lines);

	for(;;)
	{
		cleanFrame();
		buff = window(buff, pressed_key);
		pressed_key = unixGetch();
		if(buff.lines < getSize(Y) - BAR_SZ)
		{
			windowFill(buff.lines);
		}
	}
}

void argcCheck(int arg_count)
{
	if(arg_count > 2)
	{
		fputs("Fiflo can handle max. one additional arg.\n", stderr);
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	signal(SIGTSTP, sigHandler); // CTRL_X
	signal(SIGINT, sigHandler); // CTRL_C

	argcCheck(argc);
	if(argv[1] == NULL)
	{
		run("noname.asdf\0");
	}
	else if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
	{
		showHelp();
		exit(0);
	}
	else
	{
		run(argv[1]);
	}
	return 0;
}

#else

#include <stdio.h>

int main(void)
{
	fputs("Only Linux-based systems are supported, exited.\n", stderr);
	return 0;
}

#endif

