#include "buffer.h"
#include "config.h"
#include "modes.h"
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

void ui__print_line_number(Buff_t* Buffer, Conf_t* Config, const idx_t line_idx,
                           const term_t line_num_length)
{
    ui__set_color(&Config->Color_line_number);

    if(line_idx == CURRENT_LINE_IDX)
    {
        ui__set_color(&Config->Color_line_number_current);
    }
    printf("%*u ", line_num_length - SPACE_SZ, line_idx + IDX);

    ui__set_color(NULL);
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

    if((term_t) strlen(Buffer->git_branch)
       <= (Ui->win_w - GIT_LOGO_LENGTH - STATUS_MAX - SPACE_SZ))
    {
        printf("%s%*s%s", Buffer->status, Ui->win_w
               - (term_t) strlen(Buffer->git_branch)
               - (term_t) strlen(Buffer->status), GIT_LOGO, Buffer->git_branch);
    }
    else
    {
        printf("%s%*s%.*s...", Buffer->status, Ui->win_w
               - (term_t) strlen(Buffer->git_branch)
               - (term_t) strlen(Buffer->status), GIT_LOGO, Ui->win_w
               - STATUS_MAX - GIT_LOGO_LENGTH + SPACE_SZ,
               Buffer->git_branch);
    }
    WRAP_LINE();
}

void ui__lower_bar(Buff_t* Buffer, Conf_t* Config, Mod_t* Modes, Ui_t* Ui)
{
    idx_t       punch_card_width = 80;
    const idx_t tmp_width        = punch_card_width;
    char        punch_card[16];

    const char key_binding[8][STATUS_MAX] =
    {
        "CTRL^D - delete line",
        "CTRL^O - save as",
        "CTRL^Q - exit",
        "CTRL^S - save",
        "CTRL^\\ - keyboard shortcuts"
    };

    sprintf(punch_card, "%u", punch_card_width);
    WRAP_LINE();

    ui__set_color(NULL); // Resets the last line color.
    ui__set_color(&Config->Color_ui);

    if(Modes->lbar_toggled)
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

    if(Ui->text_x > punch_card_width)
    {
        printf("%*s",
               Ui->line_num_length + punch_card_width - STATUS_MAX - SPACE_SZ
               - (term_t) strlen(punch_card), " ");

        if(CURSOR_X >= Ui->text_x) // Scrolling.
        {
            punch_card_width = CURSOR_X + IDX - Ui->text_x + tmp_width;
        }

        if((CURRENT_LINE_LENGTH > punch_card_width)
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
