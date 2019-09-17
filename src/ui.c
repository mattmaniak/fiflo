#include "ui.h"

void ui__colorize(const int value)
{
    // Reset to a default color or set am another one.
    printf("\033[%um", (value == 0) ? 0 : value);
}

void ui__print_line_number(const Buff_t* const Buffer,
                           const Conf_t* const Config, const idx_t ln_idx,
                           const term_t line_num_len)
{
    ui__colorize(Config->Color_ui.value);
    ANSI__INVERT();

    if(ln_idx == BUFFER__ACTUAL_LINE_IDX)
    {
        ui__colorize(0);
        ui__colorize(Config->Color_ui.value);
    }
    printf("%*u", line_num_len - SPACE_SZ, ln_idx + IDX);

    ui__colorize(0);
    putchar(' ');
}

void ui__upper_bar(const Buff_t* const Buffer, const Conf_t* const Config,
                   const Ui_t* const Ui)
{
    const int fname_area  = Ui->win_w - UI__LEFT_PADDING - UI__RIGHT_PADDING;

    ui__colorize(Config->Color_ui.value);
    ANSI__INVERT();
    printf("%*s", UI__LEFT_PADDING, " ");

    if(Buffer->fname_len <= (const size_t) fname_area)
    {
        printf("%s%*s", Buffer->fname, (int) (fname_area
               - strlen(Buffer->fname) + UI__RIGHT_PADDING), " ");
    }
    else
    {
        // The filename is too long to show - scroll it.
        for(size_t ch_idx = Buffer->fname_len - Ui->win_w
            + UI__HORIZONTAL_PADDING; ch_idx < Buffer->fname_len; ch_idx++)
        {
            putchar(Buffer->fname[ch_idx]);
        }
        printf("%*s", UI__RIGHT_PADDING, " ");
        WRAP_LINE();
    }
    printf("%*s%s%*s", UI__LEFT_PADDING, " ", Buffer->status,
           (const int) (BUFFER__STATUS_MAX - strlen(Buffer->status) - SPACE_SZ
           + UI__GIT_LOGO_W), UI__GIT_LOGO);

    if((const term_t) strlen(Buffer->git_branch)
       < (Ui->win_w - UI__GIT_LOGO_W - BUFFER__STATUS_MAX
          - UI__HORIZONTAL_PADDING))
    {
        printf("%s%*s", Buffer->git_branch, (int) (fname_area
               - BUFFER__STATUS_MAX - strlen(Buffer->git_branch)
               - UI__GIT_LOGO_W + UI__LEFT_PADDING + UI__RIGHT_PADDING), " ");
    }
    else
    {
        printf("%.*s", Ui->win_w - BUFFER__STATUS_MAX - SPACE_SZ ,
               Buffer->git_branch);
    }
    WRAP_LINE();
}

void ui__lower_bar(const Buff_t* const Buffer, const Conf_t* const Config,
                   const Mod_t* const Modes, const Ui_t* const Ui,
                   const idx_t additional_argc_idx,
                   const idx_t actual_file_idx)
{
    const int  fname_area  = Ui->win_w - UI__LEFT_PADDING - UI__RIGHT_PADDING;
    const char files_str[] = "loaded files";
    idx_t      punch_card_w = 80;
    char       punch_card[16];
    char       cursor_pos_indicator[BUFFER__STATUS_MAX];

    sprintf(punch_card, "%u", punch_card_w);
    sprintf(cursor_pos_indicator, "[%u; %u]",
            Buffer[actual_file_idx].lines_amount
            - Buffer[actual_file_idx].cursor_rev_y + IDX,
            BUFFER__CURSOR_X + IDX);

    WRAP_LINE();
    ui__colorize(0); // Resets a last line color.
    ui__colorize(Config->Color_ui.value);

    if(Modes->expanded_lbar)
    {
        ANSI__INVERT();

        printf("%*s%s%*s", UI__LEFT_PADDING, " ", files_str, (int) (fname_area
               - strlen(files_str) + UI__RIGHT_PADDING), " ");
        WRAP_LINE();

        for(idx_t file_idx = 0; file_idx <= additional_argc_idx; file_idx++)
        {
            ui__colorize(0);
            ui__colorize(Config->Color_ui.value);
            ANSI__INVERT();

            if(file_idx == actual_file_idx)
            {
                ui__colorize(0);
                ui__colorize(Config->Color_ui.value);
            }
            printf("%*s", UI__LEFT_PADDING, " ");

            if((term_t) strlen(Buffer[file_idx].fname) <= fname_area)
            {
                printf("%s%*s", Buffer[file_idx].fname, (int) (fname_area
                       - strlen(Buffer[file_idx].fname)), " ");
            }
            else
            {
                printf("%.*s", fname_area, Buffer[file_idx].fname);
            }
            printf("%*s", UI__RIGHT_PADDING, " ");
            WRAP_LINE();
        }
        for(idx_t ln_idx = 0; ln_idx < 4 - additional_argc_idx - IDX; ln_idx++)
        {
            ui__colorize(Config->Color_ui.value + 10);
            printf("%*s", Ui->win_w, " ");
            ui__colorize(0);
            WRAP_LINE();
        }
    }
    ui__colorize(Config->Color_ui.value);
    ANSI__INVERT();

    // Cursor position indicator (2D matrix).
    printf("%*s%s%*s", UI__LEFT_PADDING, " ", cursor_pos_indicator,
           (const int) (Ui->win_w - UI__LEFT_PADDING
           - strlen(cursor_pos_indicator)), " ");
    ui__colorize(0);
}
