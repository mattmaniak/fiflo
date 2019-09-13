#include "ui.h"

void ui__set_color(const int* const value)
{
    // Reset to a default color or set am another one.
    printf("\033[%um", (value == NULL) ? 0 : *value);
}

void ui__print_line_number(const Buff_t* const Buffer,
                           const Conf_t* const Config, const idx_t ln_idx,
                           const term_t line_num_len)
{
    ui__set_color(&Config->Color_line_number.value);

    if(ln_idx == BUFFER__ACTUAL_LINE_IDX)
    {
        ui__set_color(&Config->Color_line_number_actual.value);
    }
    printf("%*u ", line_num_len - SPACE_SZ, ln_idx + IDX);

    ui__set_color(NULL);
}

void ui__upper_bar(const Buff_t* const Buffer, const Conf_t* const Config,
                   const Ui_t* const Ui)
{
    ui__set_color(&Config->Color_ui.value);
    printf("%*s", UI__LEFT_PADDING, " ");

    if(Buffer->fname_len < (const size_t) (Ui->win_w - UI__RIGHT_PADDING))
    {
        puts(Buffer->fname);
    }
    else
    {
        // The filename is too long to show - scroll it.
        for(size_t ch_idx = Buffer->fname_len - Ui->win_w
            + UI__HORIZONTAL_PADDING; ch_idx < Buffer->fname_len; ch_idx++)
        {
            putchar(Buffer->fname[ch_idx]);
        }
        WRAP_LINE();
    }
    printf("%*s%s%*s", UI__LEFT_PADDING, " ", Buffer->status,
           (const int) (STATUS_MAX - strlen(Buffer->status) - SPACE_SZ
           + UI__GIT_LOGO_W), UI__GIT_LOGO);

    if((const term_t) strlen(Buffer->git_branch)
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
                   const Mod_t* const Modes, const idx_t additional_argc_idx,
                   const idx_t actual_file_idx)
{
    idx_t punch_card_width = 80;
    char  punch_card[16];
    char  lbar_text[STATUS_MAX];

    sprintf(punch_card, "%u", punch_card_width);
    sprintf(lbar_text, "[%u; %u]",
            Buffer->lines_amount - Buffer->cursor_rev_y + IDX,
            BUFFER__CURSOR_X + IDX);

    WRAP_LINE();
    ui__set_color(NULL); // Resets a last line color.
    ui__set_color(&Config->Color_ui.value);

    if(Modes->expanded_lbar)
    {
        printf("%*sloaded files:", UI__LEFT_PADDING, "");
        WRAP_LINE();

        for(idx_t file_idx = 0; file_idx <= additional_argc_idx; file_idx++)
        {
            if(file_idx == actual_file_idx)
            {
                ui__set_color(&Config->Color_actual_file.value);
            }
            printf("%*s%s", UI__LEFT_PADDING, "", Buffer[file_idx].fname);

            ui__set_color(NULL);
            WRAP_LINE();
        }
        for(idx_t ln_idx = 0; ln_idx < 4 - additional_argc_idx - IDX; ln_idx++)
        {
            WRAP_LINE();
        }
    }
    // Cursor position indicator (2D matrix).
    printf("%*s%.*s%*s", UI__LEFT_PADDING, "", STATUS_MAX, lbar_text,
           (const int) (STATUS_MAX - strlen(lbar_text)), "");
    ui__set_color(NULL);
}
