#include "handling.h"

void ptr_check(void* ptr, const char* errmsg) {
	if(!ptr) {
		fprintf(stderr, "Can't %s, exited.\n", errmsg);
		exit(1);
	}
}

char nix_getch(void) {
	struct termios old, new;
	char key;

	tcgetattr(STDIN_FILENO, &old); // Get terminal attribiutes.
	new = old;
	new.c_lflag &= ~(ICANON | ECHO); // Disable buffered I/O and echo mode.
	
	tcsetattr(STDIN_FILENO, TCSANOW, &new); // Use new terminal I/O settings.
	key = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &old); // Restore terminal settings.

	return key;
}

void set_fname(buff dt, const char* passed) {
	if(passed[strlen(passed) - NTERM_SZ] == '/') {
		fputs("Can't open the directory as the file, exited.\n", stderr);
		exit(1);
	}

	if(passed[0] == '/') { // Is absolute path.
		if(strlen(passed) + NTERM_SZ > PATH_MAX) {
			fputs("Given path is too long, exited.\n", stderr);
			exit(1);
		}
		strcpy(dt.fname, passed); // Malloced so doesn't need 'n' for size.
	}
	else { // Relative or basename.
		const bool slash_sz = 1;

		char* abs_path = malloc(PATH_MAX); // Man 3 getcwd.
		ptr_check((getcwd(abs_path, PATH_MAX)),
		"get your current path. Can be too long\0");

		if((strlen(abs_path) + strlen(passed) + NTERM_SZ) > PATH_MAX) {
			fputs("Given filename is too long, exited.\n", stderr);
			exit(1);
		}
		strcpy(dt.fname, abs_path); // Copy the path.
		dt.fname[strlen(abs_path)] = '/'; // Add the slash between.

		for(uint16_t pos = 0; pos < strlen(passed); pos++) { // Append basename.
			strcpy(&dt.fname[strlen(abs_path) + slash_sz + pos],
			&passed[pos]);
		}
		free(abs_path);
	}
}

buff read_file(buff dt) {
	FILE* textfile = fopen(dt.fname, "r");
	char chr;

	if(textfile) {
		while((chr = getc(textfile)) != EOF) {
			dt = handle_key(dt, chr);
		}
		fclose(textfile);
	}
	else {
		dt.txt[dt.lns][dt.chrs] = NTERM;
	}
	return dt;
}

void save_file(buff dt) {
	if(access(dt.fname, F_OK) == -1) { // ... there is no file.
		int created = open(dt.fname, O_CREAT | O_EXCL | O_WRONLY, 0600);

		if(created == -1) {
			fputs("Failed to create the new file, exited.\n", stderr);
			exit(1);
		}		
	}
	FILE* textfile = fopen(dt.fname, "w");
	ptr_check(textfile, "write to the file\0");

	for(buff_t ln = 0; ln <= dt.lns; ln++) {
		fputs(dt.txt[ln], textfile);
	}
	fclose(textfile);
}

void alloc_block(buff dt) {
	if(dt.chrs_ln % MEMBLK == 0) {
		dt.txt[dt.lns] = realloc(dt.txt[dt.lns], dt.chrs_ln + MEMBLK);
		ptr_check(dt.txt[dt.lns], "realloc the new memory block for chars\0");
	}
}

void free_all(buff dt) {
	free(dt.fname);
	for(buff_t ln = 0; ln <= dt.lns; ln++) {
		free(dt.txt[ln]);
	}
	free(dt.txt);
}

buff add_char(buff dt, char key) {
	dt.chrs_ln++;
	dt.chrs++;
	alloc_block(dt);

	switch(key) {
		default:
			dt.txt[dt.lns][dt.chrs_ln - INDEX] = key;
		break;

		case TAB: // Actuall converts TAB to two spaces.
			dt.txt[dt.lns][dt.chrs_ln - INDEX] = SPACE;
			dt.chrs_ln++;

			alloc_block(dt);

			dt.txt[dt.lns][dt.chrs_ln - INDEX] = SPACE;
			dt.chrs++;
		break;			

		case LF:
			dt.txt[dt.lns][dt.chrs_ln - INDEX] = LF;
			dt.txt[dt.lns][dt.chrs_ln] = NTERM;
			dt.lns++;

			dt.chrs_ln = 0;
			dt.txt[dt.lns] = malloc(MEMBLK); // The new line.
			ptr_check(dt.txt[dt.lns], "malloc byte in the new line\0");
		break;
	}
	dt.txt[dt.lns][dt.chrs_ln] = NTERM;
	return dt;
}

buff keyboard_shortcut(buff dt, char key) {
	switch(key) {
		case NEG_CHAR:
		break;

		case CTRL_D:
			save_file(dt);
		break;

		case CTRL_X: // Frees everything and exits the program.
			free_all(dt);
			exit(0);
		break;
	}
	return dt;
}

buff alloc_chr(buff dt, char key) {
//	printf("%d", (strlen(dt.txt[dt.lns]) + NTERM_SZ) % MEMBLK);
	switch(key) {
		case NTERM: // Only for the initialization.
		break;

		case NEG_CHAR: // Eg. CTRL+C - singal.
		case CTRL_D:
		case CTRL_X:
			dt = keyboard_shortcut(dt, key);
		break;

		case BACKSPACE:
			if(dt.chrs_ln - 1 % MEMBLK == MEMBLK - 1) {
				dt.txt[dt.lns] = 
				realloc(dt.txt[dt.lns], (2 * dt.chrs_ln) - MEMBLK);
				ptr_check(dt.txt[dt.lns], "free the memory block\0");
			}
			if(dt.chrs > 0) {
				dt.chrs_ln--;
				dt.chrs--;
			}
			// TODO: FREEING OLD LINE.
			dt.txt[dt.lns][dt.chrs_ln] = NTERM;
		break;


		default:
			dt = add_char(dt, key);
		break;
	}
	return dt;
}

void limits(buff dt) {
	if(dt.lns > MAX_LNS) {
		fprintf(stderr,
		"Max. lines amount: %d, chars: %d. Got more.\n", MAX_LNS, MAX_CHRS);
		free_all(dt);
		exit(1);
	}

}

buff handle_key(buff dt, char key) {
	if(key != 033) {
		dt = alloc_chr(dt, key);
		limits(dt);
	}
	return dt;
}

