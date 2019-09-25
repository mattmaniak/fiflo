#include "ui.h"

void ui__colorize(const int value)
{
    // Reset to a default color or set am another one.
    printf("\033[%um", (value == 0) ? 0 : value);
}

void ui__print_line_number(const Buff_t* const Buffer,
                           const Conf_t* const Config, const size_t ln_i,
                           const term_t ln_num_len)
{
    ui__colorize(Config->Color_ui.value);
    ANSI__INVERT();

    if(ln_i == BUFFER__ACTUAL_LN_I)
    {
        ui__colorize(0);
        ui__colorize(Config->Color_ui.value);
    }
    printf("%*lu", ln_num_len - SIZE__SPACE, ln_i + SIZE__I);

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
        printf("%s%*s", Buffer->fname, fname_area
               - (const int) strlen(Buffer->fname) + UI__RIGHT_PADDING, " ");
    }
    else
    {
        // The filename is too long to show - scroll it.
        for(size_t ch_i = Buffer->fname_len - Ui->win_w
            + UI__HORIZONTAL_PADDING; ch_i < Buffer->fname_len; ch_i++)
        {
            putchar(Buffer->fname[ch_i]);
        }
        printf("%*s", UI__RIGHT_PADDING, " ");
        WRAP_LN();
    }
    printf("%*s%s%*s", UI__LEFT_PADDING, " ", Buffer->status,
           (BUFFER__STATUS_MAX - (const int) strlen(Buffer->status)
           - SIZE__SPACE + UI__GIT_LOGO_W), UI__GIT_LOGO);

    if((const term_t) strlen(Buffer->git_branch)
       < (Ui->win_w - UI__GIT_LOGO_W - BUFFER__STATUS_MAX
          - UI__HORIZONTAL_PADDING))
    {
        printf("%s%*s", Buffer->git_branch, (fname_area
               - BUFFER__STATUS_MAX - (const int) strlen(Buffer->git_branch)
               - UI__GIT_LOGO_W + UI__LEFT_PADDING + UI__RIGHT_PADDING), " ");
    }
    else
    {
        printf("%.*s", Ui->win_w - BUFFER__STATUS_MAX - SIZE__SPACE ,
               Buffer->git_branch);
    }
    WRAP_LN();
}

void ui__lower_bar(const Buff_t* const Buffer, const Conf_t* const Config,
                   const Mod_t* const Modes, const Ui_t* const Ui,
                   const size_t additional_argc_i,
                   const size_t actual_file_i)
{
    const int  fname_area  = Ui->win_w - UI__LEFT_PADDING - UI__RIGHT_PADDING;
    const char files_str[] = "loaded files";
    size_t      punch_card_w = 80;
    char       punch_card[16];
    char       cursor_pos_indicator[BUFFER__STATUS_MAX];

    sprintf(punch_card, "%lu", punch_card_w);
    sprintf(cursor_pos_indicator, "[%lu; %lu]",
            Buffer[actual_file_i].ln_amount
            - Buffer[actual_file_i].cursor_rev_y + SIZE__I,
            BUFFER__CURSOR_X + SIZE__I);

    WRAP_LN();
    ui__colorize(0); // Resets a last line color.
    ui__colorize(Config->Color_ui.value);

    if(Modes->expanded_lbar)
    {
        ANSI__INVERT();

        printf("%*s%s%*s", UI__LEFT_PADDING, " ", files_str, fname_area
               - (const int) strlen(files_str) + UI__RIGHT_PADDING, " ");
        WRAP_LN();

        for(size_t file_i = 0; file_i <= additional_argc_i; file_i++)
        {
            ui__colorize(0);
            ui__colorize(Config->Color_ui.value);
            ANSI__INVERT();

            if(file_i == actual_file_i)
            {
                ui__colorize(0);
                ui__colorize(Config->Color_ui.value);
            }
            printf("%*s", UI__LEFT_PADDING, " ");

            if((term_t) strlen(Buffer[file_i].fname) <= fname_area)
            {
                printf("%s%*s", Buffer[file_i].fname, fname_area
                       - (const int) strlen(Buffer[file_i].fname), " ");
            }
            else
            {
                printf("%.*s", fname_area, Buffer[file_i].fname);
            }
            printf("%*s", UI__RIGHT_PADDING, " ");
            WRAP_LN();
        }
    }
    ui__colorize(Config->Color_ui.value);
    ANSI__INVERT();

    // Cursor position indicator (2D matrix).
    printf("%*s%s%*s", UI__LEFT_PADDING, " ", cursor_pos_indicator,
           Ui->win_w - UI__LEFT_PADDING
           - (const int) strlen(cursor_pos_indicator), " ");
    ui__colorize(0);
}
