#include "buffer.h"
#include "config.h"
#include "ui.h"

void ui__print_line_number(Buff_t* Buff, Conf_t* Config, idx_t line_i,
                           term_t line_num_len)
{
	config__set_color(NULL);; // Reset a text color.
	config__set_color(&Config->Color_line_number);

	if(line_i == ACT_LINE_I)
	{
		config__set_color(&Config->Color_line_number_current);
		config__set_color(NULL);;
	}
	printf("%*u", line_num_len - (2 * SPACE_SZ), ++line_i);

	config__set_color(NULL);;
	putchar(' ');
	config__set_color(&Config->Color_text);
}

void ui__upper_bar(Buff_t* Buff, Ui_t* Ui, Conf_t* Config)
{
	const idx_t punch_card_width  = 80;
	const char  git_str[]   = "|\\ git ";
	char        punch_card[16];

	sprintf(punch_card, "%u", punch_card_width);

	/* Sometimes the empty space of width Ui->line_num_len will rendered before
	the upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. For any other output of the program CR
	is not necessary, eg. for errors messages. They can be shifted. */
	putchar('\r');
	config__set_color(&Config->Color_bar);

	if(Buff->fname_len_i < Ui->win_w)
	{
		puts(Buff->fname);
	}
	else
	{
		// TODO
		for(term_t char_i = (term_t) (Buff->fname_len_i - (size_t) (Ui->win_w
		    + CUR_SZ)); char_i < Buff->fname_len_i; char_i++)
		{
			putchar(Buff->fname[char_i]);
		}
		WRAP_LINE();
	}

	config__set_color(&Config->Color_git_logo);
	printf("%s", git_str);
	config__set_color(NULL);;

	config__set_color(&Config->Color_bar);
	printf("%s\n", Buff->git_branch);

	// The lower part with the "chars in the current line" indicator.
	printf("%.*s%*s", STATUS_MAX, Buff->status,
	       (STATUS_MAX - (term_t) strlen(Buff->status)), " ");

	if(Ui->text_x > punch_card_width) // TODO: SIMPLIFY.
	{
		printf("%*s",
		       Ui->line_num_len + punch_card_width - STATUS_MAX - SPACE_SZ,
		       " ");

		if((ACT_LINE_LEN_I > punch_card_width)
		&& (ACT_LINE[punch_card_width] != '\n'))
		{
			config__set_color(&Config->Color_warning);
		}
		printf("^%d", punch_card_width);
		config__set_color(NULL);;
	}
	else
	{
		printf("%*s", Ui->win_w - STATUS_MAX, " ");
	}
	WRAP_LINE();
	config__set_color(NULL);;
}

void ui__lower_bar(Conf_t* Config, const bool pane_toggled)
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

	config__set_color(&Config->Color_bar);
	if(pane_toggled)
	{
		for(size_t y = 0; y < TOGGLED_PANE_SZ - LBAR_SZ; y++)
		{
			printf("%s", key_binding[y]);
			WRAP_LINE();
		}
	}
	printf("%s", key_binding[TOGGLED_PANE_SZ - LBAR_SZ]);
	config__set_color(NULL);;
}
