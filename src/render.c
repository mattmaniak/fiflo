#include "render.h"

term_t get_term_sz(char axis) {
	const uint8_t x_min = 45;
	const uint8_t y_min = 4;
	const uint16_t sz_max = USHRT_MAX;

	struct winsize win; // From "sys/ioctl.h".
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);

	if(win.ws_col < x_min || win.ws_row < y_min) {
		fprintf(stderr, "Min. term size is %dx%d, exited.\n", x_min, y_min);
		exit(1);
	}
	else if(win.ws_col > sz_max || win.ws_row > sz_max) {
		fprintf(stderr, "Max. term size is %dx%d, exited.\n", sz_max, sz_max);
		exit(1);
	}

	switch(axis) {
		case 'X':
			return win.ws_col;
		case 'Y':
			return win.ws_row;
	}
	return 0; // Required -Wreturn-type.
}

void flush_window(buff dt) {
	if(dt.lns < TXT_AREA - CURRENT) {
		CURSOR_DOWN(TXT_AREA - dt.lns - CURRENT);
	}
	printf("%s", CLEAN_LINE);

	for(term_t y = 0; y <= TXT_AREA; y++) {
		printf("%s%s", LINE_UP, CLEAN_LINE);
	}
	fflush(stdout);
}

void draw_bar(buff dt, char key) {
	const char* title = "fiflo | filename:\0";
	uint16_t fname_max = get_term_sz('X') - strlen(title) - strlen(DOTS);

	printf("%s%s", INVERT, title);

	if(strlen(dt.fname) > fname_max) {
		printf(" %.*s%s\n", fname_max, dt.fname, DOTS);
	}
	else {
		printf("%s%*s\n", dt.fname, (term_t) (get_term_sz('X') - strlen(title)
		- strlen(dt.fname)), " ");
	}
	// Lower part of the draw_bar.
	printf("chars (all | line | last):%*s%*d | %*d | %*d%s\n",
	get_term_sz('X') - 45, " ", 5, dt.chrs, 5, dt.chrs_ln, 3, key, RESET);
}

void lower_fill(buff_t lns) {
	if(lns < TXT_AREA) {
		for(term_t ln = lns; ln < TXT_AREA - CURRENT; ln++) {
			putchar(LF);
		}
	}
}

void set_cursor_pos(buff dt) {
	if(dt.lns < TXT_AREA - CURRENT) {
		CURSOR_UP(TXT_AREA - dt.lns - CURRENT);

		if(strlen(dt.txt[dt.lns]) < get_term_sz('X') - strlen(DOTS)) {
			CURSOR_RIGHT((term_t) (strlen(dt.txt[dt.lns]) + strlen(DOTS)));
		}
		else {
			CURSOR_RIGHT((term_t) get_term_sz('X') - CURRENT);
		}
	}
	// Else by default on the bottom.
}

void window(buff dt, char key) { // TODO: SIMPLIFY
	const char* too_many = INVERT DOTS RESET"\0";
	buff_t hidden_lns = 0;
	draw_bar(dt, key);

	if(dt.lns >= TXT_AREA) { // Horizontal scroll.
		hidden_lns = dt.lns - TXT_AREA + CURRENT + 1; // 1 - DOTS in Y.
		puts(too_many);
	}
	for(term_t ln = hidden_lns; ln <= dt.lns; ln++) {
		printf("%s%*d%s", INVERT, (uint8_t) strlen(DOTS), ln, RESET);

		if(strlen(dt.txt[ln]) + strlen(DOTS) >= get_term_sz('X')) {
			uint8_t fill;

			if(dt.txt[ln][strlen(dt.txt[ln]) - 1] == LF) { // Move one right.
				fill = 3;
			}
			else {
				fill = 2;
			}
			printf("%s%s", too_many, dt.txt[ln] + strlen(dt.txt[ln])
			+ strlen(too_many) - get_term_sz('X') - fill);
		}
		else {
			fputs(dt.txt[ln], stdout);
		}
	}
	lower_fill(dt.lns);
	set_cursor_pos(dt);
}

