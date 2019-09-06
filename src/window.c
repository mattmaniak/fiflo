#include "window.h"

term_t window__get_terminal_sz(const char axis)
{
    const int line_sz = 1;
    const int sz_max  = USHRT_MAX;
    const int h_min   = UI__UBAR_SZ + line_sz + UI__TOGGLED_LBAR_H;
    const int w_min   = UI__GIT_LOGO_W + SPACE_SZ + UI__GIT_BRANCH_MIN_W
                        + SPACE_SZ + STATUS_MAX + UI__HORIZONTAL_PADDING;

    const struct winsize terminal;

    /* TIOCGWINSZ request to the stdout descriptor. &term is required by that
    specific device (stdout). */
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal) == ERROR)
    {
        fprintf(stderr, "Can't get the terminal's size.\n");
        return 0;
    }

    // Terminal size check.
    if((terminal.ws_col < w_min) || (terminal.ws_row < h_min))
    {
        fprintf(stderr, "Minimal terminal's size: %ux%u.\n", w_min, h_min);
        return 0;
    }
    else if((terminal.ws_col > sz_max) || (terminal.ws_row > sz_max))
    {
        fprintf(stderr, "Maximum terminal's size: %ux%u.\n", sz_max, sz_max);
        return 0;
    }

    switch(axis)
    {
    case 'X':
        return terminal.ws_col;

    case 'Y':
        return terminal.ws_row;
    }
    return 0;
}

void window__flush(void)
{
    // Restore to the left lower corner and clean the lowest line.
    ANSI_RESTORE_CURSOR_POS();
    ANSI_CLEAN_WHOLE_LINE();

    // Then from move up and clean the next lines till the window ends.
    for(term_t line = UI__LBAR_SZ; line < window__get_terminal_sz('Y'); line++)
    {
        ANSI_CURSOR_UP(1);
        ANSI_CLEAN_WHOLE_LINE();
    }
    fflush(NULL);
}

void window__fill(const Buff_t* const Buffer, const Ui_t* const Ui)
{
    // Fill the empty area below the text to pos the lower bar.
    if((Buffer->lines_amount_idx + IDX) < (idx_t) Ui->textarea_h)
    {
        for(idx_t line = Buffer->lines_amount_idx;
            line < (idx_t) (Ui->textarea_h - UI__LBAR_SZ); line++)
        {
            WRAP_LINE();
        }
    }
    // Else the lower bar will by posed by the text.
}

void window__set_cursor_pos(const Buff_t* const Buffer,
                            const Mod_t* const Modes, const Ui_t* const Ui)
{
    // Set by default to a filename edit.
    term_t move_right = (term_t) (UI__LEFT_PADDING + Buffer->fname_len);
    term_t move_up    = (term_t) (Ui->win_h - UI__LBAR_SZ);

    if(move_right >= Ui->win_w)
    {
        move_right = (term_t) (Ui->win_w - CURSOR_SZ);
    }

    // Cursor is pushed right by the lower bar. Move it back.
    ANSI_CURSOR_LEFT(Ui->win_w);
    ANSI_SAVE_CURSOR_POS();

    if(!Modes->live_fname_edit)
    {
        if(BUFFER__CURRENT_LINE_LEN < Ui->textarea_w)
        {
            // No horizontal scrolling.
            move_right = (term_t) (Ui->line_num_length + BUFFER__CURSOR_X);
        }
        else if((BUFFER__CURRENT_LINE_LEN - Ui->textarea_w)
                >= Buffer->cursor_rev_x)
        {
            /* Last Ui->textarea_w chars are seen. Current line is scrolled,
            not cursor. */
            move_right = (term_t) (Ui->win_w - CURSOR_SZ);
        }
        else
        {
            // Text is scrolled horizontally to the start. Cursor can be moved.
            move_right = (term_t) (Ui->line_num_length + BUFFER__CURSOR_X);
        }
        move_up = (BUFFER__CURRENT_LINE_IDX < Ui->textarea_h) ?
        (term_t) (Ui->textarea_h - BUFFER__CURRENT_LINE_IDX - IDX + Ui->lbar_h)
        : Ui->lbar_h;
    }
    ANSI_CURSOR_RIGHT(move_right);
    ANSI_CURSOR_UP(move_up);
}

bool window__render(const Buff_t* const Buffer, const Conf_t* const Config,
                    const Mod_t* const Modes, const idx_t additional_argc_idx,
                    const idx_t current_file_idx)
{
    char line_num_str[16]; // Needed to count the length of the number.
    Ui_t Ui;

    sprintf(line_num_str, "%u", Buffer->lines_amount_idx + IDX);

    if(((Ui.win_w = window__get_terminal_sz('X')) == 0)
       || ((Ui.win_h = window__get_terminal_sz('Y')) == 0))
    {
        return false;
    }
    Ui.toggled_lbar_h = UI__TOGGLED_LBAR_H;
    Ui.lbar_h = (Modes->lbar_expanded) ? Ui.toggled_lbar_h : UI__LBAR_SZ;

    Ui.line_num_length = (term_t) (strlen(line_num_str) + SPACE_SZ
                         + UI__LEFT_PADDING);

    Ui.textarea_w = (term_t) (Ui.win_w - Ui.line_num_length);
    Ui.textarea_h = (term_t) (Ui.win_h - UI__UBAR_SZ - Ui.lbar_h);

    ui__upper_bar(&Buffer[current_file_idx], Config, &Ui);
    print__display_text(&Buffer[current_file_idx], &Ui, Config);
    window__fill(&Buffer[current_file_idx], &Ui);

    ui__lower_bar(Buffer, Config, Modes, &Ui, additional_argc_idx,
                  current_file_idx);

    window__set_cursor_pos(&Buffer[current_file_idx], Modes, &Ui);

    return true;
}
