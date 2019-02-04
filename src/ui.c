#include "buffer.h"
#include "config.h"
#include "ui.h"

void ui__set_color(Opt_t* Option)
{
	if(Option == NULL)
	{
		printf("\033[0m"); // Reset colors and others to default.
	}
	else
	{
		printf("\033[%um", Option->value);
	}
}

void ui__print_line_number(Buff_t* Buffer, Conf_t* Config, idx_t line_i,
                           term_t line_num_length)
{
	ui__set_color(&Config->Color_line_number);

	if(line_i == CURRENT_LINE_IDX)
	{
		ui__set_color(&Config->Color_line_number_current);
	}
	printf("%*u ", line_num_length - SPACE_SZ, line_i + IDX);
	ui__set_color(NULL);

	ui__set_color(&Config->Color_text); // And naturally a line after that.
}

void ui__upper_bar(Buff_t* Buffer, Conf_t* Config, Ui_t* Ui)
{
	ui__set_color(&Config->Color_ui);

	if(Buffer->fname_length < Ui->win_w)
	{
		puts(Buffer->fname);
	}
	else
	{
		// The filename is too long to show - scroll it.
		for(size_t char_i = Buffer->fname_length - Ui->win_w + CURSOR_SZ;
		    char_i < Buffer->fname_length; char_i++)
		{
			putchar(Buffer->fname[char_i]);
		}
		WRAP_LINE();
	}
	printf("%s", GIT_LOGO);

	if((term_t) strlen(Buffer->git_branch)
	   <= (Ui->win_w - GIT_LOGO_LENGTH - STATUS_MAX - SPACE_SZ))
	{
		// The lower part with the "chars in the current line" indicator.
		printf("%s %*s", Buffer->git_branch,
		       Ui->win_w - GIT_LOGO_LENGTH - (term_t) strlen(Buffer->git_branch)
		       - SPACE_SZ,
		       Buffer->status);
	}
	else
	{
		printf("%.*s... %*s",
		       Ui->win_w - GIT_LOGO_LENGTH - SPACE_SZ - DOTS_LENGTH
		       - STATUS_MAX,
		       Buffer->git_branch,
		       STATUS_MAX,  Buffer->status);
	}
	WRAP_LINE();
}

void ui__lower_bar(Buff_t* Buffer, Conf_t* Config, Ui_t* Ui)
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

	ui__set_color(NULL); // Resets the last line color.
	ui__set_color(&Config->Color_ui);

	if(Buffer->pane_toggled)
	{
		for(size_t idx = 0; idx < (TOGGLED_PANE_SZ - LBAR_SZ); idx++)
		{
			printf("%s", key_binding[idx]);
			WRAP_LINE();
		}
	}
	printf("%.*s%*s", STATUS_MAX, key_binding[TOGGLED_PANE_SZ - LBAR_SZ],
	       STATUS_MAX - (term_t) strlen(key_binding[TOGGLED_PANE_SZ - LBAR_SZ]),
	       " ");

	if(Ui->text_x > punch_card_width) // TODO: SIMPLIFY.
	{
		printf("%*s",
		       Ui->line_num_length + punch_card_width - STATUS_MAX - SPACE_SZ
		       - (term_t) strlen(punch_card),
		       " ");


		if(CURRENT_LINE_LENGTH_IDX >= Ui->text_x) // Scrolling.
		{
			punch_card_width += CURRENT_LINE_LENGTH_IDX + IDX - Ui->text_x;
		}

		if((CURRENT_LINE_LENGTH_IDX > punch_card_width)
		&& (CURRENT_LINE[punch_card_width] != '\n'))
		{
			ui__set_color(&Config->Color_warning);
		}
		printf("%d^", punch_card_width);
	}
	else // No place for the punch card indicator.
	{
		printf("%*s", Ui->win_w - STATUS_MAX, " ");
	}
	ui__set_color(NULL);;
}
