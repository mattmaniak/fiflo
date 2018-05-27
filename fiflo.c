#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

void render(void) {
	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	uint16_t i;
	uint16_t winWidth = win.ws_col;
	uint16_t winHeight = win.ws_row;

	for(i = 0; i < winWidth; i++) {
		printf("%c", 's');
	}
	for(i = 2; i < winHeight; i++) {
		printf("%c", '\n');
	}
	for(i = 0; i < winWidth; i++) {
		printf("%c", 's');
	}
}

// *argv[] - table of pointers, (*argv)[] pointer of table.
int main(uint8_t argc, char *argv[]) {
	if(argc > 2) {
		puts("To many args!");
	}

	render();
	return 0;
}

