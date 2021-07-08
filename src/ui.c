#include "ui.h"

void ui__colorize(const int value)
{
    // Reset to a default color or set am another one.
    printf("\033[%um", value);
}

void ui__print_line_number(const V_file* const v_file,
                           const Config* const config, const size_t ln_i,
                           const term_t line_number_len)
{
    ui__colorize(config->color_ui.value);
    ANSI__INVERT();

    if(ln_i == v_file__cursor_y(v_file))
    {
        ui__colorize(0);
        ui__colorize(config->color_ui.value);
    }
    printf("%*u", line_number_len - SIZE__SPACE, (unsigned) ln_i + SIZE__I);

    ui__colorize(0);
    putchar(' ');
}

void ui__upper_bar(const V_file* const v_file, const Config* const config,
                   const Ui* const ui)
{
    const int fname_area  = ui->win_w - UI__LEFT_PADDING - UI__RIGHT_PADDING;

    ui__colorize(config->color_ui.value);
    ANSI__INVERT();
    printf("%*s", UI__LEFT_PADDING, " ");

    if(v_file->fname_len <= (size_t) fname_area)
    {
        printf("%s%*s", v_file->fname,
               fname_area - (int) strlen(v_file->fname) + UI__RIGHT_PADDING,
               " ");
    }
    else
    {
        // The filename is too long to show - scroll it.
        for(size_t char_i = v_file->fname_len - ui->win_w
            + UI__HORIZONTAL_PADDING;
            char_i < v_file->fname_len; char_i++)
        {
            putchar(v_file->fname[char_i]);
        }
        printf("%*s", UI__RIGHT_PADDING, " ");
        UI__WRAP_LINE();
    }
    printf("%*s%s%*s", UI__LEFT_PADDING, " ", v_file->status,
           V_FILE__STATUS_MAX - (int) strlen(v_file->status) - SIZE__SPACE
           + UI__GIT_LOGO_W + SIZE__I,
           UI__GIT_LOGO);

    if((term_t) strlen(v_file->git_branch)
       < (ui->win_w - UI__GIT_LOGO_W - V_FILE__STATUS_MAX
          - UI__HORIZONTAL_PADDING))
    {
        printf("%s%*s", v_file->git_branch,
               fname_area - V_FILE__STATUS_MAX - SIZE__I
               - (int) strlen(v_file->git_branch) - UI__GIT_LOGO_W
               + UI__LEFT_PADDING + UI__RIGHT_PADDING,
               " ");
    }
    else
    {
        printf("%.*s%*s", ui->win_w - V_FILE__STATUS_MAX - SIZE__SPACE - SIZE__I
               - UI__GIT_LOGO_W,
               v_file->git_branch, UI__RIGHT_PADDING, " ");
    }
    UI__WRAP_LINE();
}

void ui__lower_bar(const V_file* const v_files, const Config* const config,
                   const Modes* const modes, const Ui* const ui,
                   size_t additional_argc_i, const size_t actual_file_i)
{
    const V_file* const v_file      = &v_files[actual_file_i];
    const int           fname_area  = ui->win_w - UI__LEFT_PADDING
                                      - UI__RIGHT_PADDING;
    const char          files_str[] = "loaded files";
    char                cursor_pos_indicator[V_FILE__STATUS_MAX];


    sprintf(cursor_pos_indicator, "[%u; %u]",
            (unsigned) v_file__cursor_y(v_file) + SIZE__I,
            (unsigned) v_file__cursor_x(v_file) + SIZE__I);

    UI__WRAP_LINE();
    ui__colorize(0); // Resets a last line color.
    ui__colorize(config->color_ui.value);

    if(modes->expanded_lbar)
    {
        ANSI__INVERT();

        printf("%*s%s%*s", UI__LEFT_PADDING, " ", files_str,
               fname_area - (int) strlen(files_str) + UI__RIGHT_PADDING, " ");
        UI__WRAP_LINE();

        if(additional_argc_i == 0)
        {
            additional_argc_i++;
        }
        for(size_t file_i = 0; file_i < additional_argc_i; file_i++)
        {
            ui__colorize(0);
            ui__colorize(config->color_ui.value);
            ANSI__INVERT();

            if(file_i == actual_file_i)
            {
                ui__colorize(0);
                ui__colorize(config->color_ui.value);
            }
            printf("%*s", UI__LEFT_PADDING, " ");

            if((term_t) strlen(v_file[file_i].fname) <= fname_area)
            {
                printf("%s%*s", v_file[file_i].fname,
                       fname_area - (int) strlen(v_file[file_i].fname), " ");
            }
            else
            {
                printf("%.*s", fname_area, v_file[file_i].fname);
            }
            printf("%*s", UI__RIGHT_PADDING, " ");
            UI__WRAP_LINE();
        }
    }
    ui__colorize(config->color_ui.value);
    ANSI__INVERT();

    // Cursor position indicator (2D matrix).
    printf("%*s%s%*s", UI__LEFT_PADDING, " ", cursor_pos_indicator,
           ui->win_w - UI__LEFT_PADDING - (int) strlen(cursor_pos_indicator),
           " ");
    ui__colorize(0);
}
