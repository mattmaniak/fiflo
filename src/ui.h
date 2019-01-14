#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

// Some semantic macros.
#define CUR_SZ   1
#define LF_SZ    1
#define SLASH_SZ 1
#define SPACE_SZ 1

#define UBAR_SZ 3
#define LBAR_SZ 1 // Must be equal 1, otherwise will break rendering a little.

#define TOGGLED_PANE_SZ 5

#define WRAP_LINE() putchar('\n')

// ANSI escape codes.
#define ANSI_RESET()                     printf("\x1b[0m")
#define ANSI_BOLD()                      printf("\x1b[1m")
#define ANSI_UNDERSCORE()                printf("\x1b[4m")
#define ANSI_INVERT()                    printf("\x1b[7m")

#define ANSI_RED()                       printf("\x1b[31m")
#define ANSI_GREEN()                     printf("\x1b[32m")
#define ANSI_YELLOW()                    printf("\x1b[33m")
#define ANSI_BLUE()                      printf("\x1b[34m")
#define ANSI_MAGENTA()                   printf("\x1b[35m")
#define ANSI_CYAN()                      printf("\x1b[36m")
#define ANSI_WHITE()                     printf("\x1b[37m")
#define ANSI_RED_BRIGHT()                printf("\x1b[91m")
#define ANSI_GREEN_BRIGHT()              printf("\x1b[92m")
#define ANSI_YELLOW_BRIGHT()             printf("\x1b[93m")
#define ANSI_BLUE_BRIGHT()               printf("\x1b[94m")
#define ANSI_MAGENTA_BRIGHT()            printf("\x1b[95m")
#define ANSI_CYAN_BRIGHT()               printf("\x1b[96m")
#define ANSI_WHITE_BRIGHT()              printf("\x1b[97m")

#define RESET          "\x1b[0m"
#define BOLD           "\x1b[1m"
#define RED            "\x1b[31m"
#define GREEN          "\x1b[32m"
#define YELLOW         "\x1b[33m"
#define BLUE           "\x1b[34m"
#define MAGENTA        "\x1b[35m"
#define CYAN           "\x1b[36m"
#define WHITE          "\x1b[37m"
#define RED_BRIGHT     "\x1b[91m"
#define GREEN_BRIGHT   "\x1b[92m"
#define YELLOW_BRIGHT  "\x1b[93m"
#define BLUE_BRIGHT    "\x1b[94m"
#define MAGENTA_BRIGHT "\x1b[95m"
#define CYAN_BRIGHT    "\x1b[96m"
#define WHITE_BRIGHT   "\x1b[97m"

#define ANSI_RED_BACKGROUND()            printf("\x1b[41m")
#define ANSI_GREEN_BACKGROUND()          printf("\x1b[42m")
#define ANSI_YELLOW_BACKGROUND()         printf("\x1b[43m")
#define ANSI_BLUE_BACKGROUND()           printf("\x1b[44m")
#define ANSI_MAGENTA_BACKGROUND()        printf("\x1b[45m")
#define ANSI_CYAN_BACKGROUND()           printf("\x1b[46m")
#define ANSI_WHITE_BACKGROUND()          printf("\x1b[47m")
#define ANSI_RED_BRIGHT_BACKGROUND()     printf("\x1b[101m")
#define ANSI_GREEN_BRIGHT_BACKGROUND()   printf("\x1b[102m")
#define ANSI_YELLOW_BRIGHT_BACKGROUND()  printf("\x1b[103m")
#define ANSI_BLUE_BRIGHT_BACKGROUND()    printf("\x1b[104m")
#define ANSI_MAGENTA_BRIGHT_BACKGROUND() printf("\x1b[105m")
#define ANSI_CYAN_BRIGHT_BACKGROUND()    printf("\x1b[106m")
#define ANSI_WHITE_BRIGHT_BACKGROUND()   printf("\x1b[107m")

#define RED_BACKGROUND            "\x1b[41m"
#define GREEN_BACKGROUND          "\x1b[42m"
#define YELLOW_BACKGROUND         "\x1b[43m"
#define BLUE_BACKGROUND           "\x1b[44m"
#define MAGENTA_BACKGROUND        "\x1b[45m"
#define CYAN_BACKGROUND           "\x1b[46m"
#define WHITE_BACKGROUND          "\x1b[47m"
#define RED_BRIGHT_BACKGROUND     "\x1b[101m"
#define GREEN_BRIGHT_BACKGROUND   "\x1b[102m"
#define YELLOW_BRIGHT_BACKGROUND  "\x1b[103m"
#define BLUE_BRIGHT_BACKGROUND    "\x1b[104m"
#define MAGENTA_BRIGHT_BACKGROUND "\x1b[105m"
#define CYAN_BRIGHT_BACKGROUND    "\x1b[106m"
#define WHITE_BRIGHT_BACKGROUND   "\x1b[107m"

#define ANSI_CLEAN_LINE()                printf("\x1b[2K")
#define ANSI_CUR_UP(offset)              printf("\x1b[%dA", offset)
#define ANSI_CUR_DOWN(offset)            printf("\x1b[%dB", offset)
#define ANSI_CUR_RIGHT(offset)           printf("\x1b[%dC", offset)
#define ANSI_CUR_LEFT(offset)            printf("\x1b[%ldD", offset)
#define ANSI_SAVE_CUR_POS()              printf("\x1b[s")
#define ANSI_RESTORE_CUR_POS()           printf("\x1b[u")

typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

typedef struct
{
	char   line_num_str[16]; // Place for string of the highest line number.
	size_t line_num_len;     // Dynamic width of the lines numbers.
	term_t text_x;           // Horizontal space for the text (width: chars).
	term_t text_y;           // Vertical space for the text (lines).
	term_t lbar_h;           // Lower bar height (lines).
	term_t pane_h;           // As above but toggled.

	size_t win_w;
	size_t win_h;
}
Ui_t;

// Prints the line number.
void ui_print_line_number(idx_t line_i, term_t line_num_len,
                          const bool current_line);

#endif
