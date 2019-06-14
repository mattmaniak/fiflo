#include "ui.h"

void ui__set_color(const Opt_t* const Option)
{
    // Reset to the default color or set am another one.
    printf("\033[%um", (Option == NULL) ? 0 : Option->value);
}

void ui__print_line_number(const Buff_t* const Buffer,
                           const Conf_t* const Config, const idx_t line_idx,
                           const term_t line_num_length)
{
    ui__set_color(&Config->Color_line_number);

    if(line_idx == BUFFER__CURRENT_LINE_IDX)
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
    printf("%*s", UI__LEFT_PADDING, " ");

    if(Buffer->fname_length < (size_t) (Ui->win_w - UI__RIGHT_PADDING))
    {
        puts(Buffer->fname);
    }
    else
    {
        // The filename is too long to show - scroll it.
        for(size_t char_i = Buffer->fname_length - Ui->win_w
            + UI__HORIZONTAL_PADDING; char_i < Buffer->fname_length; char_i++)
        {
            putchar(Buffer->fname[char_i]);
        }
        WRAP_LINE();
    }
    printf("%*s%s%*s", UI__LEFT_PADDING, " ", Buffer->status, (int) (STATUS_MAX
           - strlen(Buffer->status) - SPACE_SZ + UI__GIT_LOGO_W), UI__GIT_LOGO);

    if((term_t) strlen(Buffer->git_branch)
       < (Ui->win_w - UI__GIT_LOGO_W - STATUS_MAX - UI__HORIZONTAL_PADDING))
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
                   const Mod_t* const Modes, const Ui_t* const Ui,
                   const idx_t additional_argc_idx)
{
    idx_t       punch_card_width = 80;
    const idx_t tmp_width        = punch_card_width;
    char        punch_card[16];

    const char key_binding[] = " CTRL^\\ - keyboard shortcuts";

    sprintf(punch_card, "%u", punch_card_width);
    WRAP_LINE();

    ui__set_color(NULL); // Resets the last line color.
    ui__set_color(&Config->Color_ui);

    if(Modes->lbar_toggled)
    {
        printf("%*sloaded files:", UI__LEFT_PADDING, "");
        WRAP_LINE();

        for(idx_t file_idx = 0; file_idx < additional_argc_idx; file_idx++)
        {
            printf("%*s%s", UI__LEFT_PADDING, "", Buffer[file_idx].fname);
            WRAP_LINE();
        }
        for(idx_t newline_idx = 0; newline_idx < 4 - additional_argc_idx;
            newline_idx++)
        {
            WRAP_LINE();
        }
    }
    printf("%.*s%*s", STATUS_MAX, key_binding, STATUS_MAX
           - (term_t) strlen(key_binding), " ");

    if((idx_t) (Ui->textarea_w + UI__HORIZONTAL_PADDING) > punch_card_width)
    {
        printf("%*s",
               Ui->line_num_length + punch_card_width - STATUS_MAX - SPACE_SZ
               - (term_t) strlen(punch_card), " ");

        if(BUFFER__CURSOR_X >= Ui->textarea_w) // Scrolling.
        {
            punch_card_width = BUFFER__CURSOR_X + IDX - Ui->textarea_w
                               + tmp_width;
        }

        if((BUFFER__CURRENT_LINE_LENGTH > punch_card_width)
           && (BUFFER__CURRENT_LINE[punch_card_width] != '\n'))
        {
            ui__set_color(&Config->Color_warning);
        }
        printf("%d^", punch_card_width);
    }
    ui__set_color(NULL);;
}
