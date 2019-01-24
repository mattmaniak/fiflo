#include "buffer.h"
#include "config.h"
#include "ui.h"

void ui__print_line_number(Buff_t* Buff, Conf_t* Config, idx_t line_i,
                           term_t line_num_len)
{
	if(line_i == ACT_LINE_I)
	{
		config__set_color(&Config->Color_line_number_current);
	}
	printf("%*u ", line_num_len - SPACE_SZ, ++line_i);
	config__set_color(NULL);;
}

void ui__upper_bar(Buff_t* Buff, Conf_t* Config, Ui_t* Ui)
{
	/* Sometimes the empty space of width Ui->line_num_len will rendered before
	the upper bar. Adding the carriage return before it fixes the problems. Just
	handling with terminals' quirk modes. For any other output of the program CR
	is not necessary, eg. for errors messages. They can be shifted. */
	// putchar('\r');

	if(Buff->fname_len_i < Ui->win_w)
	{
		puts(Buff->fname);
	}
	else
	{
		// The filename is too long to show - scroll it.
		for(size_t char_i = (Buff->fname_len_i - Ui->win_w + CUR_SZ);
		    char_i < Buff->fname_len_i; char_i++)
		{
			putchar(Buff->fname[char_i]);
		}
		WRAP_LINE();
	}
	config__set_color(&Config->Color_git_logo);
	printf("%s", GIT_LOGO);
	config__set_color(NULL);;

	if((term_t) strlen(Buff->git_branch)
	   <= (Ui->win_w - GIT_LOGO_LENGTH - STATUS_MAX - SPACE_SZ))
	{
		// The lower part with the "chars in the current line" indicator.
		printf("%s %*s", Buff->git_branch,
		       Ui->win_w - GIT_LOGO_LENGTH - (term_t) strlen(Buff->git_branch) - SPACE_SZ,
		       Buff->status);
	}
	else
	{
		printf("%.*s... ",
		       (int) (Ui->win_w - GIT_LOGO_LENGTH - DOTS_LENGTH), Buff->git_branch);

		printf("%s", Buff->status);
	}
	WRAP_LINE();
	config__set_color(NULL);;
}

void ui__lower_bar(Buff_t* Buff, Conf_t* Config, Ui_t* Ui)
{
	idx_t punch_card_width = 80;
	char  punch_card[16];

	const char key_binding[8][STATUS_MAX] =
	{
		"CTRL^D - delete line",
		"CTRL^O - save as",
		"CTRL^Q - exit",
		"CTRL^O - save",
		"CTRL^\\ - keyboard shortcuts"
	};

	sprintf(punch_card, "%u", punch_card_width);

	WRAP_LINE();

	config__set_color(NULL);
	if(Buff->pane_toggled)
	{
		for(size_t y = 0; y < (TOGGLED_PANE_SZ - LBAR_SZ); y++)
		{
			printf("%s", key_binding[y]);
			WRAP_LINE();
		}
	}
	printf("%.*s%*s", STATUS_MAX, key_binding[TOGGLED_PANE_SZ - LBAR_SZ],
	       STATUS_MAX - (term_t) strlen(key_binding[TOGGLED_PANE_SZ - LBAR_SZ]),
	       " ");

	if(Ui->text_x > punch_card_width) // TODO: SIMPLIFY.
	{
		printf("%*s",
		       Ui->line_num_len + punch_card_width - STATUS_MAX - SPACE_SZ
		       - (term_t) strlen(punch_card),
		       " ");


		if(ACT_LINE_LEN_I >= Ui->text_x) // Scrolling.
		{
			punch_card_width += ACT_LINE_LEN_I + INDEX - Ui->text_x;
		}

		if((ACT_LINE_LEN_I > punch_card_width)
		&& (ACT_LINE[punch_card_width] != '\n'))
		{
			config__set_color(&Config->Color_warning);
		}
		printf("%d^", punch_card_width);
		config__set_color(NULL);;
	}
	else // No place for the punch card indicator.
	{
		printf("%*s", Ui->win_w - STATUS_MAX, " ");
	}
	config__set_color(NULL);;
}
