#include "ui.h"

void ui__set_color(const Opt_t* const Option)
{
    // Reset to the default color or set another.
    printf("\033[%um", (Option == NULL) ? 0 : Option->value);
}

void ui__print_line_number(const Buff_t* const Buffer,
                           const Conf_t* const Config, const idx_t line_idx,
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

void ui__upper_bar(const Buff_t* const Buffer, const Conf_t* const Config,
                   const Ui_t* const Ui)
{
    ui__set_color(&Config->Color_ui);
    printf("%*s", LEFT_PADDING, " ");

    if(Buffer->fname_length < (size_t) (Ui->win_w - RIGHT_PADDING))
    {
        puts(Buffer->fname);
    }
    else
    {
        // The filename is too long to show - scroll it.
        for(size_t char_i = Buffer->fname_length - Ui->win_w
            + HORIZONTAL_PADDING; char_i < Buffer->fname_length; char_i++)
        {
            putchar(Buffer->fname[char_i]);
        }
        WRAP_LINE();
    }
    printf("%*s%s%*s", LEFT_PADDING, " ", Buffer->status, (int) (STATUS_MAX
           - strlen(Buffer->status) - SPACE_SZ), GIT_LOGO);

    if((term_t) strlen(Buffer->git_branch)
       < (Ui->win_w - GIT_LOGO_LENGTH - STATUS_MAX - HORIZONTAL_PADDING))
    {
        printf("%s", Buffer->git_branch);
    }
    else
    {
        printf("%.*s", Ui->win_w - STATUS_MAX - SPACE_SZ , Buffer->git_branch);
    }
    WRAP_LINE();
}

void ui__lower_bar(const Buff_t* const Buffer, const Conf_t* const Config,
                   const Mod_t* const Modes, const Ui_t* const Ui)
{
    idx_t       punch_card_width = 80;
    const idx_t tmp_width        = punch_card_width;
    char        punch_card[16];

    const char key_binding[8][STATUS_MAX] =
    {
        " CTRL^D - delete line",
        " CTRL^O - save as",
        " CTRL^Q - exit",
        " CTRL^S - save",
        " CTRL^\\ - keyboard shortcuts"
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

    if((idx_t) (Ui->text_x + HORIZONTAL_PADDING) > punch_card_width)
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
    ui__set_color(NULL);;
}
