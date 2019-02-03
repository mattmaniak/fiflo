#ifdef __linux__

#include "buffer.h"
#include "config.h"
#include "fiflo.h"

void fiflo__run(const char* arg)
{
	char pressed_key = '\0'; // Initializer only.

	Buff_t Buffer;
	Conf_t Config;

	if(!buffer__init(&Buffer))
	{
		goto free;
	}
	if(!file__load_config(&Config))
	{
		goto free;
	}
	if(!file__set_name(&Buffer, arg))
	{
		goto free;
	}
	if(!file__load(&Buffer, &Config))
	{
		goto free;
	}
	strncpy(Buffer.orig_fname, Buffer.fname, PATH_MAX);

	for(;;) // The main program loop.
	{
		if(!file__get_git_branch(&Buffer))
		{
			break;
		}
		if(!input__parse_key(&Buffer, &Config, pressed_key))
		{
			break;
		}
		// Flushes and renders always after the keypress.
		if(!window__render(&Buffer, &Config))
		{
			break;
		}
		if((pressed_key = input__getch()) == ERROR)
		{
			break;
		}
		window__flush();
	}

	free:
	buffer__free(&Buffer);
}

int main(int argc, char** argv)
{
	const int max_arg_count = 2;

	if(argc > max_arg_count)
	{
		fprintf(stderr, "Only one additional arg can be passed.\n");
		goto exit;
	}

	if(argv[1] == NULL)	// Sets the default basename and starts.
	{
		fiflo__run("");
		goto exit;
	}
	else if(strlen(argv[1]) <= PATH_MAX)
	{
		if(!options__parse_and_print(argv[1]))
		{
			goto exit;
		}
		fiflo__run(argv[1]);
		goto exit;
	}
	else
	{
		fprintf(stderr, "Maximum parameter length is %u\n", PATH_MAX);
		goto exit;
	}

	exit:
	fflush(NULL);
	return 0;
}

#else
#error "GNU/Linux is required."
#endif
