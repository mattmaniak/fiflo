#ifndef UI_H
#define UI_H

// Some semantic macros.
#define CUR_SZ   1
#define LF_SZ    1
#define SLASH_SZ 1
#define SPACE_SZ 1

// TODO: IN WIN_MTDT.
#define UBAR_SZ 2
#define LBAR_SZ 1 // Must be equal 1 otherwise will break rendering a little.
#define BARS_SZ (UBAR_SZ + LBAR_SZ)

// Text that is shown on the lower bar. Also defines the minimal terminal width.
#define LBAR_STR "CTRL^: D - delete line; O - save as; Q - exit; S - save\0"

#define WRAP_LINE() putchar(0x0a)

typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

#pragma pack(push, 1)
typedef struct
{
	char    line_num_str[16]; // Place for string of the highest line number.
	uint8_t line_num_len;     // Dynamic width of the lines numbers.
	term_t  text_x;           // Horizontal space for the text (width: chars).
	term_t  text_y;           // Vertical space for the text (lines).
	char    lbar_str[60];
	uint8_t ubar_sz;
	uint8_t lbar_sz;
}
win_mtdt;
#pragma pack(pop)

// buffer.h
extern _Noreturn void free_buff_exit(f_mtdt* Buff, const bool code);


#endif
