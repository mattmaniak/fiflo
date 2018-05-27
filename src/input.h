#include <termios.h>
#include <unistd.h>

int unixGetch(void) { // https://stackoverflow.com/questions/12710582/
	struct termios oldt,newt;
	int8_t character;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	character = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return character; }

