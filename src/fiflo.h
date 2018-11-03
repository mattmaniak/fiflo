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

// Some semantic substitutes.
#define INDEX  1
#define NUL_SZ 1
#define LF     10

#define BUF_MAX     (UINT_MAX / 256) // Currently 16 MB - 1 buffer limit.
#define INIT_MEMBLK sizeof(Dt->text) // Aligned initial memblk for a new line.
#define MEMBLK      128              // Must be >= 16 and dividable by 8.

/* Because strlen("16777216") = 8. "+ 1" is the right padding. Setting a value
like 1234 won't be good idea. */
#define STRLEN_BUF_T    (8 + 1)
#define STATUS_MAX      32
#define SET_STATUS(msg) strncpy(Dt->status, msg, STATUS_MAX)

typedef uint32_t buf_t;  // Only for amount indicators.
typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

#pragma pack(push, 4)
typedef struct
{
	FILE*  textf;              // File handle.
	char   fname[PATH_MAX];    // Full filename. Eg. /home/user/basename.

	char** text;               // Text buffer. Eg. text[lines][chars].
	buf_t  lines;              // Lines index.
	buf_t* line_len;           // Chars in the current line (index).
	buf_t  chars;              // All chars index.

	buf_t  cusr_x;             // User's cursor position in mirrored X.
	char   status[STATUS_MAX]; // Displayed message in the upper bar.
}
meta;
#pragma pack(pop)

// Needed to simplify and shorten the code.
#define ACT_LN      Dt->text[Dt->lines]
#define ACT_LN_LEN  Dt->line_len[Dt->lines]
#define PREV_LN     Dt->text[Dt->lines - 1]
#define PREV_LN_LEN Dt->line_len[Dt->lines - 1]

// From api.
extern meta* set_fname(const char* arg, meta* Dt);
extern meta* read_file(meta* Dt);

// From keys.
extern meta* recognize_key(char key, meta* Dt);

// From render.
extern void flush_window(meta* Dt);
extern void window(meta* Dt);

// Frees everything and exits with status code.
_Noreturn void free_all_exit(_Bool code, meta* Dt);

// Signal catcher that does nothing.
void ignore_sig(int sig_num);

// Checks if passed pointer is NULL. If yes - frees memory and exits.
void chk_ptr(void* ptr, const char* err_msg, meta* Dt);

// Program parameters, eg. "--help".
void options(const char* arg);

// Reads one char wthout confirming by the ENTER key.
char getch(void);

// Initializes all Dt structure members.
meta* init(const char* arg, meta* Dt);

// Some initializers and the main program loop.
_Noreturn void run(const char* arg);

int main(int argc, char** argv);

#endif

