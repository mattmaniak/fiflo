#include "buffer.h"
#include "config.h"
#include "ui.h"

void ui_print_line_number(Conf_t* Config, idx_t line_i, term_t line_num_len,
                          const bool current_line)
{
	ANSI_RESET(); // Reset a text color.
	putchar(' ');
	ANSI_MAGENTA_BRIGHT();

	if(current_line) // Higlight a current line.
	{
		config_set_color(&Config->Color_current_line_number);
		ANSI_WHITE();
	}
	printf("%*u", line_num_len - (2 * SPACE_SZ), ++line_i);

	ANSI_RESET();
	putchar(' ');
	config_set_color(&Config->Color_text);
}

void ui_upper_bar(Buff_t* Buff, Ui_t* Ui, Conf_t* Config)
{
	const idx_t punch_card  = 80;
	const char  git_str[]   = "|\\ git ";
	char        punch_card_str[16];

	sprintf(punch_card_str, "%u", punch_card);

	/* Sometimes the empty space of width Ui->line_num_len will rendered before
	the upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. For any other output of the program CR
	is not necessary, eg. for errors messages. They can be shifted. */
	printf("\r ");
	config_set_color(&Config->Color_text);

	if((Buff->fname_len_i + SPACE_SZ) < Ui->win_w)
	{
		puts(Buff->fname);
	}
	else
	{
		for(term_t char_i = (term_t) (Buff->fname_len_i - (size_t) (Ui->win_w
		    + CUR_SZ + SPACE_SZ)); char_i < Buff->fname_len_i; char_i++)
		{
			putchar(Buff->fname[char_i]);
		}
		putchar(' ');
		WRAP_LINE();
	}

	config_set_color(&Config->Color_git_logo);
	printf(" %s", git_str);
	ANSI_RESET();

	config_set_color(&Config->Color_text);
	printf("%s\n", Buff->git_branch);

	// The lower part with the "chars in the current line" indicator.
	printf(" %.*s%*s", STATUS_MAX, Buff->status,
	       (STATUS_MAX - (term_t) strlen(Buff->status) - SPACE_SZ), " ");

	if(Ui->text_x > punch_card) // TODO: SIMPLIFY.
	{
		printf("%*s", Ui->line_num_len + punch_card - STATUS_MAX - SPACE_SZ, " ");

		if((ACT_LINE_LEN_I > punch_card) && (ACT_LINE[punch_card] != '\n'))
		{
			config_set_color(&Config->Color_warning);
		}
		printf("^%d", punch_card);
		ANSI_RESET();
	}
	else
	{
		printf("%*s", Ui->win_w - STATUS_MAX, " ");
	}
	WRAP_LINE();
	ANSI_RESET();
}

void ui_lower_bar(Conf_t* Config, const bool pane_toggled)
{
	const char key_binding[8][STATUS_MAX] =
	{
		"CTRL^D - delete line",
		"CTRL^O - save as",
		"CTRL^Q - exit",
		"CTRL^O - save",
		"CTRL^\\ - keyboard shortcuts"
	};
	WRAP_LINE();

	config_set_color(&Config->Color_bar);
	if(pane_toggled)
	{
		for(size_t y = 0; y < TOGGLED_PANE_SZ - LBAR_SZ; y++)
		{
			printf(" %s", key_binding[y]);
			WRAP_LINE();
		}
	}
	printf(" %s", key_binding[TOGGLED_PANE_SZ - LBAR_SZ]);
	ANSI_RESET();
}
