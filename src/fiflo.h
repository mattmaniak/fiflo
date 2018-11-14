#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <termios.h>
#include <unistd.h>

#define DEBUG // Uncomment if You want to get variables values et al.

// Some semantic substitutes.
#define INDEX  1
#define NUL_SZ 1
#define LF     10

#define BUFF_MAX (UINT_MAX / 256) // (16 MB - 1) buffer limit.

/* Because strlen("16777216") = 8. "+ 1" is the right padding. Setting a value
like 1234 won't be good idea. */
//#define STRLEN_BUF_T    (8 + 1)
#define STATUS_MAX      40
#define SET_STATUS(msg) strncpy(Buff->status, msg, STATUS_MAX)

typedef uint32_t buff_t; // Only for amount indicators.
typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

#pragma pack(push, 4)
typedef struct
{
	char    fname[PATH_MAX];    // Full filename. Eg. /home/user/basename.

	// File's content and some indicators.
	char**  text;               // Text buffer. Eg. text[lines][chars].
	buff_t  lines;              // Lines index.
	buff_t* line_len;           // Chars in the current line (index).
	buff_t  chars;              // All chars index.

	// Editing feedback.
	buff_t  cusr_x;             // User's cursor position in the reversed X.
	buff_t  cusr_y;             // As above but Y-axis.
	char    status[STATUS_MAX]; // Displayed message in the upper bar.
}
f_mtdt;
#pragma pack(pop)

// Needed to simplify and shorten the code.
#define PREV_LN     Buff->text[Buff->lines - Buff->cusr_y - 1]
#define PREV_LN_LEN Buff->line_len[Buff->lines - Buff->cusr_y - 1]
#define ACT_LN      Buff->text[Buff->lines - Buff->cusr_y]
#define ACT_LN_LEN  Buff->line_len[Buff->lines - Buff->cusr_y]
#define LAST_LN     Buff->text[Buff->lines]
#define LAST_LN_LEN Buff->line_len[Buff->lines]

// file.h
extern f_mtdt* set_fname(f_mtdt* Buff, const char* passed);
extern f_mtdt* read_file(f_mtdt* Buff);

// text.h
extern f_mtdt* recognize_key(f_mtdt* Buff, char key);

// render.h
extern void flush_window (f_mtdt* Buff);
extern void render_window(f_mtdt* Buff);

// Frees everything and exits with status code.
_Noreturn void free_all_exit(f_mtdt* Buff, _Bool code);

// Signal catcher that does nothing.
void ignore_sig(int sig_num);

// Checks if passed pointer is NULL. If yes - frees memory and exits.
void chk_ptr(f_mtdt* Buff, void* ptr, const char* err_msg);

// Program parameters, eg. "--help".
void options(const char* arg);

// Reads one char wthout confirming by the ENTER key.
char getch(void);

// Initializes all Buff structure members.
f_mtdt* init(f_mtdt* Buff, const char* arg);

// Some initializers and the main program loop.
_Noreturn void run(const char* arg);

int main(int argc, char** argv);

#endif

